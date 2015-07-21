#pragma once

#include <Windows.h>
#include <comdef.h>
#include <atlbase.h>

#include "DXAudio.h"
#include "DXWindow.h"

#include "WindowCallback.h"
#include "StreamCallback.h"

class App {
public:
	int Run();

private:
	CComPtr<IDXWindow> m_Window;
	CComPtr<IDXAudioStream> m_Stream;

	WindowCallback m_WindowCallback;
	StreamCallback m_StreamCallback;
};