#pragma once

#include <Windows.h>
#include <comdef.h>
#include <atlbase.h>
#include <d3d11.h>

#include "DXWindow.h"
#include "QueryInterface.h"

class WindowCallback : public CDXWindowCallback {
public:
	WindowCallback() : run(true) { }

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

	VOID CreateDevice();

	VOID Render();

	bool run;

	CComPtr<ID3D11Device> m_Device;
	CComPtr<ID3D11DeviceContext> m_DeviceContext;
};