#include "AudioAnalysis.h"
#include "CAudioAnalysis.h"
#include <atlbase.h>

#pragma comment(lib, "libfftwf-3.3.lib")

//Create the audio analysis object
HRESULT AudioAnalysisCreate (
	const AUDIO_ANALYSIS_DESC* pDesc,
	IAudioAnalysisCallback* pAudioAnalysisCallback,
	IAudioAnalysis** ppAudioAnalysis
) {
	if (pDesc == nullptr || pAudioAnalysisCallback == nullptr ||
		ppAudioAnalysis == nullptr) {
		return E_POINTER;
	}

	HRESULT hr = S_OK;

	CComPtr<CAudioAnalysis> Analysis = new CAudioAnalysis();

	hr = Analysis->Initialize(*pDesc, pAudioAnalysisCallback);

	if (FAILED(hr)) {
		*ppAudioAnalysis = nullptr;
		return hr;
	}

	*ppAudioAnalysis = Analysis;

	return S_OK;
}