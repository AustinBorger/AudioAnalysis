#include "WindowCallback.h"
#include <string>

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
}

VOID WindowCallback::Render() {

}