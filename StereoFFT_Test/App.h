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

#include "DXAudio.h"
#include "DXWindow.h"

#include "WindowCallback.h"
#include "StreamCallback.h"

class App {
public:
	App();

	int Run();

private:
	WindowCallback m_WindowCallback;
	StreamCallback m_StreamCallback;
	CComPtr<IDXWindow> m_Window;
	CComPtr<IDXAudioStream> m_Stream;
};