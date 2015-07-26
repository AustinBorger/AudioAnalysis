/*
** Copyright (C) 2015 Austin Borger <aaborger@gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
** API documentation is available here:
**		https://github.com/AustinBorger/StereoFFT
*/

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

	VOID STDMETHODCALLTYPE OnKeyDown(IDXWindow* pWindow, WPARAM Key, LPARAM Flags) final;

	VOID CreateDevice();

	VOID Render();

	bool run;

	enum SHADER_TYPE {
		SHADER_TYPE_MID,
		SHADER_TYPE_LEFTRIGHT,
		SHADER_TYPE_SIDE
	} m_ShaderType;

	CComPtr<IStereoFFT> m_StereoFFT;

	StreamCallback& m_StreamCallback;
	CComPtr<ID3D11Device> m_Device;
	CComPtr<ID3D11DeviceContext> m_DeviceContext;
	CComPtr<ID3D11RenderTargetView> m_RenderTargetView;
	CComPtr<ID3D11VertexShader> m_VertexShader;
	CComPtr<ID3D11Buffer> m_TimeBuffer;

	CComPtr<ID3D11Buffer> m_TransformBuffer;
	CComPtr<ID3D11Buffer> m_DoubleTransformBuffer;

	CComPtr<ID3D11PixelShader> m_MaximaShader;
	CComPtr<ID3D11PixelShader> m_AllShader;
	CComPtr<ID3D11PixelShader> m_LeftRightAll;
	CComPtr<ID3D11PixelShader> m_LeftRightMaxima;

	LARGE_INTEGER liFrequency;
	LARGE_INTEGER liOld;

	double m_SamplesNeeded;
};