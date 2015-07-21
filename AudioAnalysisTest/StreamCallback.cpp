#include "StreamCallback.h"
#include <string>

#define FILENAME L"StreamCallback.cpp"
#define HANDLE_HR(Line) if (FAILED(hr)) OnObjectFailure(FILENAME, Line, hr)

VOID StreamCallback::OnObjectFailure(LPCWSTR File, UINT Line, HRESULT hr) {
	_com_error e(hr);

	std::wstring Message;
	Message.append(File);
	Message.append(L" @ Line ");
	Message.append(std::to_wstring(Line));
	Message.append(L":\n\n");
	Message.append(e.ErrorMessage());

	MessageBoxW(NULL, Message.c_str(), L"StreamCallback::OnObjectFailure()", MB_OK | MB_ICONERROR);
	ExitProcess(hr);
}

VOID StreamCallback::OnProcess(FLOAT SampleRate, PFLOAT Buffer, UINT BufferFrames) {

}