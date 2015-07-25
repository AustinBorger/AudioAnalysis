#include "WindowCallback.h"
#include <string>

#include "FullscreenTriangleVS.h"
#include "MaximaPS.h"
#include "AllPS.h"
#include "LeftRightAllPS.h"

#define FILENAME L"WindowCallback.cpp"
#define HANDLE_HR(Line) if (FAILED(hr)) OnObjectFailure(FILENAME, Line, hr)

VOID WindowCallback::OnObjectFailure(LPCWSTR File, UINT Line, HRESULT hr) {
	_com_error e(hr);

	std::wstring Message;
	Message.append(File);
	Message.append(L" @ Line ");
	Message.append(std::to_wstring(Line));
	Message.append(L":\n\n");
	Message.append(e.ErrorMessage());

	MessageBoxW(NULL, Message.c_str(), L"WindowCallback::OnObjectFailure()", MB_OK | MB_ICONERROR);
	ExitProcess(hr);
}

VOID WindowCallback::OnBackBufferCreate(IDXWindow* pWindow) {
	HRESULT hr = S_OK;
	CComPtr<ID3D11Texture2D> Texture;

	pWindow->GetBackBuffer(IID_PPV_ARGS(&Texture));

	hr = m_Device->CreateRenderTargetView (
		Texture,
		nullptr,
		&m_RenderTargetView
	); HANDLE_HR(__LINE__);

	D3D11_VIEWPORT Viewport;
	D3D11_TEXTURE2D_DESC TextureDesc;

	Texture->GetDesc(&TextureDesc);

	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 0.0f;
	
	Viewport.Width = FLOAT(TextureDesc.Width);
	Viewport.Height = FLOAT(TextureDesc.Height);

	ID3D11RenderTargetView* rtv = m_RenderTargetView;

	m_DeviceContext->RSSetViewports(1, &Viewport);
	m_DeviceContext->OMSetRenderTargets(1, &rtv, nullptr);
}

VOID WindowCallback::OnBackBufferRelease(IDXWindow* pWindow) {
	m_RenderTargetView.Release();
}

VOID WindowCallback::CreateDevice() {
	HRESULT hr = S_OK;

	hr = D3D11CreateDevice (
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&m_Device,
		nullptr,
		&m_DeviceContext
	); HANDLE_HR(__LINE__);

	hr = m_Device->CreateVertexShader (
		FullscreenTriangleVS,
		sizeof(FullscreenTriangleVS),
		nullptr,
		&m_VertexShader
	); HANDLE_HR(__LINE__);

	hr = m_Device->CreatePixelShader (
		AllPS,
		sizeof(AllPS),
		nullptr,
		&m_AllShader
	); HANDLE_HR(__LINE__);

	hr = m_Device->CreatePixelShader (
		MaximaPS,
		sizeof(MaximaPS),
		nullptr,
		&m_MaximaShader
	); HANDLE_HR(__LINE__);

	hr = m_Device->CreatePixelShader (
		LeftRightAllPS,
		sizeof(LeftRightAllPS),
		nullptr,
		&m_LeftRightAll
	); HANDLE_HR(__LINE__);

	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.ByteWidth = sizeof(FLOAT) * 512;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = NULL;
	BufferDesc.StructureByteStride = 0;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	hr = m_Device->CreateBuffer (
		&BufferDesc,
		nullptr,
		&m_TransformBuffer
	); HANDLE_HR(__LINE__);

	BufferDesc.ByteWidth = sizeof(FLOAT) * 512 * 2;

	hr = m_Device->CreateBuffer (
		&BufferDesc,
		nullptr,
		&m_DoubleTransformBuffer
	); HANDLE_HR(__LINE__);

	BufferDesc.ByteWidth = sizeof(FLOAT) * 8;

	hr = m_Device->CreateBuffer (
		&BufferDesc,
		nullptr,
		&m_TimeBuffer
	); HANDLE_HR(__LINE__);

	STEREO_FFT_DESC TransformDesc;
	TransformDesc.NumSamples = 1024;

	hr = CreateStereoFFT(&TransformDesc, &m_StereoFFT);
	HANDLE_HR(__LINE__);

	QueryPerformanceFrequency(&liFrequency);
	QueryPerformanceCounter(&liOld);

	ID3D11Buffer* Buffers[] = {
		m_TransformBuffer,
		m_TimeBuffer
	};

	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
	m_DeviceContext->PSSetConstantBuffers(0, 2, Buffers);
	m_DeviceContext->PSSetShader(m_AllShader, nullptr, 0);

	m_ShaderType = SHADER_TYPE_MID;
}

VOID WindowCallback::Render() {
	HRESULT hr = S_OK;

	FLOAT ClearColor[] = {
		1.0f, 1.0f, 1.0f, 1.0f
	};

	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, ClearColor);

	LARGE_INTEGER liNew;
	QueryPerformanceCounter(&liNew);

	double time = double(liNew.QuadPart) / double(liFrequency.QuadPart);
	double dt = double(liNew.QuadPart - liOld.QuadPart) / double(liFrequency.QuadPart);
	liOld = liNew;

	m_SamplesNeeded += dt * 44100.0;
	UINT nSamplesNeeded = UINT(m_SamplesNeeded);
	m_SamplesNeeded -= nSamplesNeeded;
	nSamplesNeeded = min(1024, nSamplesNeeded);

	FLOAT l_Buffer[2048];
	m_StreamCallback.Read(l_Buffer, nSamplesNeeded);

	m_StereoFFT->Post(l_Buffer, nSamplesNeeded);
	m_StereoFFT->Process();

	D3D11_MAPPED_SUBRESOURCE Sub;

	if (m_ShaderType == SHADER_TYPE_MID) {
		hr = m_DeviceContext->Map(m_TransformBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &Sub); HANDLE_HR(__LINE__);
		memcpy(Sub.pData, m_StereoFFT->GetMidTransform(), sizeof(FLOAT) * 512);
		m_DeviceContext->Unmap(m_TransformBuffer, 0);
	} else if (m_ShaderType == SHADER_TYPE_LEFTRIGHT) {
		hr = m_DeviceContext->Map(m_DoubleTransformBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &Sub); HANDLE_HR(__LINE__);
		memcpy(Sub.pData, m_StereoFFT->GetLeftTransform(), sizeof(FLOAT) * 512);
		memcpy((BYTE*)Sub.pData + sizeof(FLOAT) * 512, m_StereoFFT->GetRightTransform(), sizeof(FLOAT) * 512);
		m_DeviceContext->Unmap(m_DoubleTransformBuffer, 0);
	}

	hr = m_DeviceContext->Map(m_TimeBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &Sub); HANDLE_HR(__LINE__);
	*((FLOAT*)(Sub.pData)) = FLOAT(time);
	m_DeviceContext->Unmap(m_TimeBuffer, 0);

	m_DeviceContext->Draw(3, 0);
}

VOID WindowCallback::OnKeyDown(IDXWindow* pWindow, WPARAM Key, LPARAM Flags) {
	ID3D11Buffer* Buffers[] = {
		nullptr,
		m_TimeBuffer
	};

	if (Key == VK_F1) {
		m_DeviceContext->PSSetShader(m_AllShader, nullptr, 0);
		Buffers[0] = m_TransformBuffer;
		m_ShaderType = SHADER_TYPE_MID;
	} else if (Key == VK_F2) {
		m_DeviceContext->PSSetShader(m_MaximaShader, nullptr, 0);
		Buffers[0] = m_TransformBuffer;
		m_ShaderType = SHADER_TYPE_MID;
	} else if (Key == VK_F3) {
		m_DeviceContext->PSSetShader(m_LeftRightAll, nullptr, 0);
		Buffers[0] = m_DoubleTransformBuffer;
		m_ShaderType = SHADER_TYPE_LEFTRIGHT;
	}

	m_DeviceContext->PSSetConstantBuffers(0, 2, Buffers);
}