#pragma once

#include <Windows.h>
#include <comdef.h>
#include <atlbase.h>

#include "DXAudio.h"
#include "QueryInterface.h"

class StreamCallback : public CDXAudioWriteCallback {
public:
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
		QUERY_INTERFACE_CAST(IDXAudioWriteCallback);
		QUERY_INTERFACE_CAST(IDXAudioCallback);
		QUERY_INTERFACE_CAST(IUnknown);
		QUERY_INTERFACE_FAIL();
	}

	ULONG STDMETHODCALLTYPE AddRef() {
		return 1;
	}

	ULONG STDMETHODCALLTYPE Release() {
		return 1;
	}

	VOID STDMETHODCALLTYPE OnObjectFailure(LPCWSTR File, UINT Line, HRESULT hr) final;

	VOID STDMETHODCALLTYPE OnProcess(FLOAT SampleRate, PFLOAT Buffer, UINT BufferFrames) final;
};