#include <Windows.h>
#include "App.h"

#pragma comment(lib, "DXAudio.lib")
#pragma comment(lib, "DXWindow.lib")
#pragma comment(lib, "AudioAnalysis.lib")
#pragma comment(lib, "d3d11.lib")

int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
	App app;

	return app.Run();
}