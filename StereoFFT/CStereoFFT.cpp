#include "CStereoFFT.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define FILENAME L"CStereoFFT.cpp"
#define FFTWF_FREE(x) if (x != nullptr) { fftwf_free(x); x = nullptr; }

CStereoFFT::CStereoFFT() :
m_RefCount(1),
m_NumSamples(0),
m_ComplexBuffer(nullptr),
m_ProcessBuffer(nullptr),
m_Window(nullptr),
m_Plan(nullptr)
{ }

CStereoFFT::~CStereoFFT() {
	FFTWF_FREE(m_HistoryBuffer);
	FFTWF_FREE(m_ComplexBuffer);
	FFTWF_FREE(m_ProcessBuffer);
	FFTWF_FREE(m_Window);

	if (m_Plan != nullptr) {
		fftwf_destroy_plan(m_Plan);
		m_Plan = nullptr;
	}

	FFTWF_FREE(m_LeftMix);
	FFTWF_FREE(m_RightMix);
	FFTWF_FREE(m_MidMix);
	FFTWF_FREE(m_SideMix);

	FFTWF_FREE(m_LeftTransform);
	FFTWF_FREE(m_RightTransform);
	FFTWF_FREE(m_MidTransform);
	FFTWF_FREE(m_SideTransform);
}

HRESULT CStereoFFT::Initialize(const STEREO_FFT_DESC& Desc) {
	m_NumSamples = Desc.NumSamples;

	m_HistoryBuffer = fftwf_alloc_real(m_NumSamples * 2);
	m_ProcessBuffer = fftwf_alloc_real(m_NumSamples);
	m_ComplexBuffer = fftwf_alloc_complex(m_NumSamples / 2 + 1);
	m_Window = fftwf_alloc_real(m_NumSamples);

	m_Plan = fftwf_plan_dft_r2c_1d (
		m_NumSamples,
		m_ProcessBuffer,
		m_ComplexBuffer,
		0
	);

	m_LeftMix = fftwf_alloc_real(m_NumSamples);
	m_RightMix = fftwf_alloc_real(m_NumSamples);
	m_MidMix = fftwf_alloc_real(m_NumSamples);
	m_SideMix = fftwf_alloc_real(m_NumSamples);

	m_LeftTransform = fftwf_alloc_real(m_NumSamples / 2);
	m_RightTransform = fftwf_alloc_real(m_NumSamples / 2);
	m_SideTransform = fftwf_alloc_real(m_NumSamples / 2);
	m_MidTransform = fftwf_alloc_real(m_NumSamples / 2);

	//Use 7-term blackman harris
	float a0 = 0.27105140069342f;
	float a1 = 0.43329793923448f;
	float a2 = 0.21812299954311f;
	float a3 = 0.06592544638803f;
	float a4 = 0.01081174209837f;
	float a5 = 0.00077658482522f;
	float a6 = 0.00001388721735f;

	static const UINT N = m_NumSamples;

	for (UINT n = 0; n < N; n++) {
		m_Window[n] = a0;
		m_Window[n] -= a1 * cos((1.0f * 2.0f * FLOAT(M_PI) * n) / N);
		m_Window[n] += a2 * cos((2.0f * 2.0f * FLOAT(M_PI) * n) / N);
		m_Window[n] -= a3 * cos((3.0f * 2.0f * FLOAT(M_PI) * n) / N);
		m_Window[n] += a4 * cos((4.0f * 2.0f * FLOAT(M_PI) * n) / N);
		m_Window[n] -= a5 * cos((5.0f * 2.0f * FLOAT(M_PI) * n) / N);
		m_Window[n] += a6 * cos((6.0f * 2.0f * FLOAT(M_PI) * n) / N);
	}

	return S_OK;
}

VOID CStereoFFT::Post(PFLOAT Buffer, UINT BufferFrames) {
	INT NumOld = max(0, m_NumSamples - BufferFrames);
	INT NumNew = min(m_NumSamples, BufferFrames);
	
	if (NumOld > 0) {
		memcpy(m_HistoryBuffer, m_HistoryBuffer + NumNew * 2, NumOld * 2 * sizeof(FLOAT));
	}

	if (NumNew > 0) {
		memcpy(m_HistoryBuffer + NumOld * 2, Buffer, NumNew * 2 * sizeof(FLOAT));
	}
}

VOID CStereoFFT::Process() {
	GenerateLeft();
	GenerateRight();
	GenerateMid();
	GenerateSide();

	ProcessMix(m_LeftMix, m_LeftTransform, m_LeftDC);
	ProcessMix(m_RightMix, m_RightTransform, m_RightDC);
	ProcessMix(m_MidMix, m_MidTransform, m_MidDC);
	ProcessMix(m_SideMix, m_SideTransform, m_SideDC);
}

VOID CStereoFFT::GenerateLeft() {
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_LeftMix[n] = m_HistoryBuffer[n * 2];
	}
}

VOID CStereoFFT::GenerateRight() {
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_RightMix[n] = m_HistoryBuffer[n * 2 + 1];
	}
}

VOID CStereoFFT::GenerateMid() {
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_MidMix[n] = m_HistoryBuffer[n * 2];
		m_MidMix[n] += m_HistoryBuffer[n * 2 + 1];
		m_MidMix[n] /= 2.0f;
	}
}

VOID CStereoFFT::GenerateSide() {
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_SideMix[n] = m_HistoryBuffer[n * 2];
		m_SideMix[n] -= m_HistoryBuffer[n * 2 + 1];
		m_SideMix[n] /= 2.0f;
	}
}

VOID CStereoFFT::ProcessMix(PFLOAT Mix, PFLOAT Transform, FLOAT& DC) {
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_ProcessBuffer[n] = Mix[n];
		m_ProcessBuffer[n] *= m_Window[n];
	}

	fftwf_execute(m_Plan);

	DC = pow(m_ComplexBuffer[0][0], 2) + pow(m_ComplexBuffer[0][1], 2);

	for (UINT n = 0; n < m_NumSamples / 2; n++) {
		Transform[n] = pow(m_ComplexBuffer[n + 1][0], 2) + pow(m_ComplexBuffer[n + 1][1], 2);
	}
}