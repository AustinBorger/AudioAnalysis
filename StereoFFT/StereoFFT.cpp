#include "StereoFFT.h"
#include "CStereoFFT.h"
#include <atlbase.h>

#pragma comment(lib, "libfftwf-3.3.lib")

//Create the audio analysis object
HRESULT CreateStereoFFT (
	const STEREO_FFT_DESC* pDesc,
	IStereoFFT** ppStereoFFT
) {
	if (pDesc == nullptr || ppStereoFFT == nullptr) {
		return E_POINTER;
	}

	HRESULT hr = S_OK;

	CComPtr<CStereoFFT> StereoFFT = new CStereoFFT();

	hr = StereoFFT->Initialize(*pDesc);

	if (FAILED(hr)) {
		*ppStereoFFT = nullptr;
		return hr;
	}

	*ppStereoFFT = StereoFFT;

	return S_OK;
}