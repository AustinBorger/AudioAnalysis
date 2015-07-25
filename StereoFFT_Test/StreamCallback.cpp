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