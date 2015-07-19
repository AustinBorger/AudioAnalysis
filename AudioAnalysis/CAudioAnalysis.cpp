#include "CAudioAnalysis.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define FFTWF_FREE(x) if (x != nullptr) { fftwf_free(x); x = nullptr; }

CAudioAnalysis::CAudioAnalysis() :
m_RefCount(1),
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

	FFTWF_FREE(m_MonoMix);
	FFTWF_FREE(m_LeftMix);
	FFTWF_FREE(m_RightMix);
	FFTWF_FREE(m_MiddleMix);
	FFTWF_FREE(m_SideMix);

	FFTWF_FREE(m_MonoTransform);
	FFTWF_FREE(m_LeftTransform);
	FFTWF_FREE(m_RightTransform);
	FFTWF_FREE(m_MiddleTransform);
	FFTWF_FREE(m_SideTransform);
}

HRESULT CAudioAnalysis::Initialize(const AUDIO_ANALYSIS_DESC& Desc) {
	m_HistoryBuffer = fftwf_alloc_real(IDEAL_BUFFER_SIZE * 2);
	m_ProcessBuffer = fftwf_alloc_real(IDEAL_BUFFER_SIZE);
	m_ComplexBuffer = fftwf_alloc_complex(IDEAL_BUFFER_SIZE / 2 + 1);
	m_Window = fftwf_alloc_real(IDEAL_BUFFER_SIZE);
	m_Plan = fftwf_plan_dft_r2c_1d(IDEAL_BUFFER_SIZE, m_ProcessBuffer, m_ComplexBuffer, 0);

	m_MonoMix = fftwf_alloc_real(IDEAL_BUFFER_SIZE);
	m_LeftMix = fftwf_alloc_real(IDEAL_BUFFER_SIZE);
	m_RightMix = fftwf_alloc_real(IDEAL_BUFFER_SIZE);
	m_MiddleMix = fftwf_alloc_real(IDEAL_BUFFER_SIZE);
	m_SideMix = fftwf_alloc_real(IDEAL_BUFFER_SIZE);

	m_MonoTransform = fftwf_alloc_real(IDEAL_BUFFER_SIZE / 2);
	m_LeftTransform = fftwf_alloc_real(IDEAL_BUFFER_SIZE / 2);
	m_RightTransform = fftwf_alloc_real(IDEAL_BUFFER_SIZE / 2);
	m_MiddleTransform = fftwf_alloc_real(IDEAL_BUFFER_SIZE / 2);
	m_SideTransform = fftwf_alloc_real(IDEAL_BUFFER_SIZE / 2);

	//Use Blackman-Nuttall
	float a0 = 0.3635819f;
	float a1 = 0.4891775f;
	float a2 = 0.1365995f;
	float a3 = 0.0106411f;

	static const UINT N = IDEAL_BUFFER_SIZE;

	for (UINT n = 0; n < N; n++) {
		m_Window[n] = a0;
		m_Window[n] -= (a1 * cos((1.0f * 2.0f * M_PI * n) / (N - 1)));
		m_Window[n] += (a2 * cos((2.0f * 2.0f * M_PI * n) / (N - 1)));
		m_Window[n] -= (a3 * cos((3.0f * 2.0f * M_PI * n) / (N - 1)));
	}
}

VOID CAudioAnalysis::Post(PFLOAT Buffer, UINT BufferFrames) {
	INT NumOld = max(0, IDEAL_BUFFER_SIZE - BufferFrames);
	INT NumNew = min(IDEAL_BUFFER_SIZE, BufferFrames);
	
	memcpy(m_HistoryBuffer, m_HistoryBuffer + NumNew * 2, NumOld * 2 * sizeof(FLOAT));
	memcpy(m_HistoryBuffer + NumOld * 2, Buffer, NumNew * 2 * sizeof(FLOAT));
}

VOID CAudioAnalysis::Process() {
	GenerateMono();
	GenerateLeft();
	GenerateRight();
	GenerateMiddle();
	GenerateSide();

	ProcessMix(m_MonoMix, m_MonoTransform);
	ProcessMix(m_LeftMix, m_LeftTransform);
	ProcessMix(m_RightMix, m_RightTransform);
	ProcessMix(m_MiddleMix, m_MiddleTransform);
	ProcessMix(m_SideMix, m_SideTransform);
}

VOID CAudioAnalysis::GenerateMono() {
	for (UINT n = 0; n < IDEAL_BUFFER_SIZE; n++) {
		m_MonoMix[n] = m_HistoryBuffer[n * 2];
		m_MonoMix[n] += m_HistoryBuffer[n * 2 + 1];
		m_MonoMix[n] /= 2.0f;
	}
}

VOID CAudioAnalysis::GenerateLeft() {
	for (UINT n = 0; n < IDEAL_BUFFER_SIZE; n++) {
		m_LeftMix[n] = m_HistoryBuffer[n * 2];
	}
}

VOID CAudioAnalysis::GenerateRight() {
	for (UINT n = 0; n < IDEAL_BUFFER_SIZE; n++) {
		m_RightMix[n] = m_HistoryBuffer[n * 2 + 1];
	}
}

VOID CAudioAnalysis::GenerateMiddle() {
	for (UINT n = 0; n < IDEAL_BUFFER_SIZE; n++) {
		m_MiddleMix[n] = m_HistoryBuffer[n * 2];
		m_MiddleMix[n] += m_HistoryBuffer[n * 2 + 1];
		m_MiddleMix[n] /= sqrt(2.0f);
	}
}

VOID CAudioAnalysis::GenerateSide() {
	for (UINT n = 0; n < IDEAL_BUFFER_SIZE; n++) {
		m_SideMix[n] = m_HistoryBuffer[n * 2];
		m_SideMix[n] -= m_HistoryBuffer[n * 2 + 1];
		m_SideMix[n] /= sqrt(2.0f);
	}
}

VOID CAudioAnalysis::ProcessMix(PFLOAT Mix, PFLOAT Transform) {
	memcpy(m_ProcessBuffer, Mix, IDEAL_BUFFER_SIZE * sizeof(FLOAT));

	fftwf_execute(m_Plan);

	float power = 0.0f;
	float db = 0.0f;

	for (UINT n = 0; n < IDEAL_BUFFER_SIZE / 2; n++) {
		power = pow(m_ComplexBuffer[n + 1][0], 2) + pow(m_ComplexBuffer[n + 1][1], 2);
		db = 10.0f * log10f(1.0f + power);
		Transform[n] = db;
	}
}