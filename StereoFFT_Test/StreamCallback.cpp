#include "StreamCallback.h"
#include <string>

#define FILENAME L"StreamCallback.cpp"
#define HANDLE_HR(Line) if (FAILED(hr)) OnObjectFailure(FILENAME, Line, hr)

StreamCallback::StreamCallback() {
	for (UINT i = 0; i < 4096; i++) {
		m_Queue.push({ 0, 0 });
	}
}

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
	m_Mutex.lock();

	for (UINT i = 0; i < BufferFrames; i++) {
		m_Queue.push({ Buffer[i * 2], Buffer[i * 2 + 1] });
	}

	if (m_Queue.size() > 4096) {
		for (UINT i = 0; i < BufferFrames; i++) {
			m_Queue.pop();
		}
	}

	m_Mutex.unlock();
}

VOID StreamCallback::Read(PFLOAT Dst, UINT BufferFrames) {
	m_Mutex.lock();

	for (UINT i = 0; i < BufferFrames; i++) {
		Dst[i * 2] = m_Queue.front().Left;
		Dst[i * 2 + 1] = m_Queue.front().Right;
		m_Queue.pop();
	}

	m_Mutex.unlock();
}