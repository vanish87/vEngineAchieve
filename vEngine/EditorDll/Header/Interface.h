#pragma once
#include <windows.h>

extern "C"
{
	__declspec(dllexport) void InitD3D(HWND hWnd, int Width, int Height);    // sets up and initializes Direct3D
}

extern "C"
{
	__declspec(dllexport) void RenderFrame(void);     // renders a single frame
}

extern "C"
{
	__declspec(dllexport) void CleanD3D(void);        // closes Direct3D and releases memory
}