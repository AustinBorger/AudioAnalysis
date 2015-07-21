#include "App.h"

#define FILENAME L"App.cpp"
#define HANDLE_HR(Line) if(FAILED(hr)) m_WindowCallback.OnObjectFailure(FILENAME, Line, hr)

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
	WindowDesc.WindowState = DXWINDOW_WINDOW_STATE_WINDOWED;

	m_WindowCallback.CreateDevice();

	hr = DXWindowCreateWindow (
		&WindowDesc,
		m_WindowCallback.m_Device,
		&m_WindowCallback,
		&m_Window
	); HANDLE_HR(__LINE__);

	DXAUDIO_STREAM_DESC StreamDesc;

	StreamDesc.SampleRate = 44100.0f;
	StreamDesc.Type = DXAUDIO_STREAM_TYPE_OUTPUT;

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