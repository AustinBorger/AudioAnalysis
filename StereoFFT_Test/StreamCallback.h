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