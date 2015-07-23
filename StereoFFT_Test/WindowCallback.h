#pragma once

#include <Windows.h>
#include <comdef.h>
#include <atlbase.h>
#include <d3d11.h>

#include "DXWindow.h"
#include "QueryInterface.h"
#include "StreamCallback.h"

#include "StereoFFT.h"

class WindowCallback : public CDXWindowCallback {
public:
	WindowCallback(StreamCallback& Callback) : m_StreamCallback(Callback), run(true), m_SamplesNeeded(0.0) { }

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) final {
		QUERY_INTERFACE_CAST(IDXWindowCallback);
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

	VOID STDMETHODCALLTYPE OnWindowClose(IDXWindow* pWindow) final {
		run = false;
	}

	VOID STDMETHODCALLTYPE OnBackBufferCreate(IDXWindow* pWindow) final;

	VOID STDMETHODCALLTYPE OnBackBufferRelease(IDXWindow* pWindow) final;

	VOID CreateDevice();

	VOID Render();

	bool run;

	CComPtr<IStereoFFT> m_StereoFFT;

	StreamCallback& m_StreamCallback;
	CComPtr<ID3D11Device> m_Device;
	CComPtr<ID3D11DeviceContext> m_DeviceContext;
	CComPtr<ID3D11RenderTargetView> m_RenderTargetView;
	CComPtr<ID3D11VertexShader> m_VertexShader;
	CComPtr<ID3D11PixelShader> m_PixelShader;
	CComPtr<ID3D11Buffer> m_TransformBuffer;

	LARGE_INTEGER liFrequency;
	LARGE_INTEGER liOld;

	double m_SamplesNeeded;
};