#include "CAudioAnalysis.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define FILENAME L"CAudioAnalysis.cpp"
#define FFTWF_FREE(x) if (x != nullptr) { fftwf_free(x); x = nullptr; }
#define CHECK_ALLOC(x, Line) if (x == nullptr) { m_Callback->OnObjectFailure(FILENAME, Line, E_FAIL); return E_FAIL; }

CAudioAnalysis::CAudioAnalysis() :
m_RefCount(1),
m_NumSamples(0),
m_ComplexBuffer(nullptr),
m_ProcessBuffer(nullptr),
m_Window(nullptr),
m_Plan(nullptr)
{ }

CAudioAnalysis::~CAudioAnalysis() {
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

HRESULT CAudioAnalysis::Initialize(const AUDIO_ANALYSIS_DESC& Desc, CComPtr<IAudioAnalysisCallback> Callback) {
	m_Callback = Callback;
	m_NumSamples = Desc.NumSamples;

	m_HistoryBuffer = fftwf_alloc_real(m_NumSamples * 2); CHECK_ALLOC(m_HistoryBuffer, __LINE__);
	m_ProcessBuffer = fftwf_alloc_real(m_NumSamples); CHECK_ALLOC(m_ProcessBuffer, __LINE__);
	m_ComplexBuffer = fftwf_alloc_complex(m_NumSamples / 2 + 1); CHECK_ALLOC(m_ComplexBuffer, __LINE__);
	m_Window = fftwf_alloc_real(m_NumSamples); CHECK_ALLOC(m_Window, __LINE__);

	m_Plan = fftwf_plan_dft_r2c_1d (
		m_NumSamples,
		m_ProcessBuffer,
		m_ComplexBuffer,
		0
	); CHECK_ALLOC(m_Plan, __LINE__);

	m_LeftMix = fftwf_alloc_real(m_NumSamples); CHECK_ALLOC(m_LeftMix, __LINE__);
	m_RightMix = fftwf_alloc_real(m_NumSamples); CHECK_ALLOC(m_RightMix, __LINE__);
	m_MidMix = fftwf_alloc_real(m_NumSamples); CHECK_ALLOC(m_MidMix, __LINE__);
	m_SideMix = fftwf_alloc_real(m_NumSamples); CHECK_ALLOC(m_SideMix, __LINE__);

	m_LeftTransform = fftwf_alloc_real(m_NumSamples / 2); CHECK_ALLOC(m_LeftTransform, __LINE__);
	m_RightTransform = fftwf_alloc_real(m_NumSamples / 2); CHECK_ALLOC(m_RightTransform, __LINE__);
	m_SideTransform = fftwf_alloc_real(m_NumSamples / 2); CHECK_ALLOC(m_SideTransform, __LINE__);
	m_MidTransform = fftwf_alloc_real(m_NumSamples / 2); CHECK_ALLOC(m_MidTransform, __LINE__);

	//Use Blackman-Nuttall
	float a0 = 0.3635819f;
	float a1 = 0.4891775f;
	float a2 = 0.1365995f;
	float a3 = 0.0106411f;

	static const UINT N = m_NumSamples;

	for (UINT n = 0; n < N; n++) {
		m_Window[n] = a0;
		m_Window[n] -= (a1 * cos((1.0f * 2.0f * FLOAT(M_PI) * n) / (N - 1)));
		m_Window[n] += (a2 * cos((2.0f * 2.0f * FLOAT(M_PI) * n) / (N - 1)));
		m_Window[n] -= (a3 * cos((3.0f * 2.0f * FLOAT(M_PI) * n) / (N - 1)));
	}

	return S_OK;
}

VOID CAudioAnalysis::Post(PFLOAT Buffer, UINT BufferFrames) {
	INT NumOld = max(0, m_NumSamples - BufferFrames);
	INT NumNew = min(m_NumSamples, BufferFrames);
	
	memcpy(m_HistoryBuffer, m_HistoryBuffer + NumNew * 2, NumOld * 2 * sizeof(FLOAT));
	memcpy(m_HistoryBuffer + NumOld * 2, Buffer, NumNew * 2 * sizeof(FLOAT));
}

VOID CAudioAnalysis::Process() {
	GenerateLeft();
	GenerateRight();
	GenerateMid();
	GenerateSide();

	ProcessMix(m_LeftMix, m_LeftTransform, m_LeftDC);
	ProcessMix(m_RightMix, m_RightTransform, m_RightDC);
	ProcessMix(m_MidMix, m_MidTransform, m_MidDC);
	ProcessMix(m_SideMix, m_SideTransform, m_SideDC);
}

VOID CAudioAnalysis::GenerateLeft() {
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_LeftMix[n] = m_HistoryBuffer[n * 2];
	}
}

VOID CAudioAnalysis::GenerateRight() {
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_RightMix[n] = m_HistoryBuffer[n * 2 + 1];
	}
}

VOID CAudioAnalysis::GenerateMid() {
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_MidMix[n] = m_HistoryBuffer[n * 2];
		m_MidMix[n] += m_HistoryBuffer[n * 2 + 1];
		m_MidMix[n] /= 2.0f;
	}
}

VOID CAudioAnalysis::GenerateSide() {
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_SideMix[n] = m_HistoryBuffer[n * 2];
		m_SideMix[n] -= m_HistoryBuffer[n * 2 + 1];
		m_SideMix[n] /= 2.0f;
	}
}

VOID CAudioAnalysis::ProcessMix(PFLOAT Mix, PFLOAT Transform, FLOAT& DC) {
	memcpy(m_ProcessBuffer, Mix, m_NumSamples * sizeof(FLOAT));

	fftwf_execute(m_Plan);

	DC = pow(m_ComplexBuffer[0][0], 2) + pow(m_ComplexBuffer[0][1], 2);

	for (UINT n = 0; n < m_NumSamples / 2; n++) {
		Transform[n] = pow(m_ComplexBuffer[n + 1][0], 2) + pow(m_ComplexBuffer[n + 1][1], 2);
	}
}