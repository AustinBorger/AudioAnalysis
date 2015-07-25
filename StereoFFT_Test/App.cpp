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

#include "App.h"

#define FILENAME L"App.cpp"
#define HANDLE_HR(Line) if(FAILED(hr)) m_WindowCallback.OnObjectFailure(FILENAME, Line, hr)

App::App() :
m_WindowCallback(m_StreamCallback)
{ }

int App::Run() {
	HRESULT hr = S_OK;
	DXWINDOW_DESC WindowDesc;

	WindowDesc.AllowToggle = TRUE;
	WindowDesc.Cursor = NULL;
	WindowDesc.FullscreenState = DXWINDOW_FULLSCREEN_STATE_FULLSCREEN;
	WindowDesc.Height = 720;
	WindowDesc.IconLarge = NULL;
	WindowDesc.IconSmall = NULL;
	WindowDesc.InitFullscreen = FALSE;
	WindowDesc.Instance = GetModuleHandleW(NULL);
	WindowDesc.Title = L"AudioAnalysisTest";
	WindowDesc.Width = 1280;
	WindowDesc.WindowState = DXWINDOW_WINDOW_STATE_BORDERLESS;

	m_WindowCallback.CreateDevice();

	hr = DXWindowCreateWindow (
		&WindowDesc,
		m_WindowCallback.m_Device,
		&m_WindowCallback,
		&m_Window
	); HANDLE_HR(__LINE__);

	DXAUDIO_STREAM_DESC StreamDesc;

	StreamDesc.SampleRate = 44100.0f;
	StreamDesc.Type = DXAUDIO_STREAM_TYPE_LOOPBACK;

	hr = DXAudioCreateStream (
		&StreamDesc,
		&m_StreamCallback,
		&m_Stream
	); HANDLE_HR(__LINE__);

	m_Stream->Start();

	while (m_WindowCallback.run) {
		m_Window->PumpMessages();

		m_WindowCallback.Render();

		m_Window->Present(1, 0);
	}

	m_Stream->Stop();

	return 0;
}