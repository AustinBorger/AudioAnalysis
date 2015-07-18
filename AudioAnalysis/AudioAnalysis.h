#pragma once

#include <Windows.h>
#include <comdef.h>

struct AUDIO_ANALYSIS_DESC {
	FLOAT SampleRate;
};

struct __declspec(uuid("6ec6f647-b334-40d5-ab37-45b5b76ba4ac")) IAudioAnalysis : public IUnknown {

};

#ifdef _AUDIO_ANALYSIS_DLL_PROJECT
	#define _AUDIO_ANALYSIS_EXPORT_TAG __declspec(dllexport)
#else
	#define _AUDIO_ANALYSIS_EXPORT_TAG __declspec(dllimport)
#endif

extern "C" HRESULT _AUDIO_ANALYSIS_EXPORT_TAG AudioAnalysisCreate (
	const AUDIO_ANALYSIS_DESC* pDesc,
	IAudioAnalysis** ppAudioAnalysis
);