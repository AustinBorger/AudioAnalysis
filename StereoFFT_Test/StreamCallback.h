#pragma once

#include <Windows.h>
#include <comdef.h>
#include <atlbase.h>
#include <mutex>
#include <queue>

#include "DXAudio.h"
#include "QueryInterface.h"

class StreamCallback : public CDXAudioReadCallback {
public:
	StreamCallback();

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
		QUERY_INTERFACE_CAST(IDXAudioReadCallback);
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

	VOID Read(PFLOAT Dst, UINT BufferFrames);

private:
	struct Sample {
		FLOAT Left;
		FLOAT Right;
	};

	std::mutex m_Mutex;
	std::queue<Sample> m_Queue;
};