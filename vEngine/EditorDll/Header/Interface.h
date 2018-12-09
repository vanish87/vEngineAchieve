#pragma once
#include <windows.h>

#include "Header/AppWrapper.h"

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib") //may fix in the future
#pragma comment(lib, "dxguid.lib") 
#pragma comment(lib, "d3dcompiler.lib") 

static MyApp app;

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

extern "C"
{
	__declspec(dllexport) void InitApp(HWND hWnd);    // sets up and initializes engine app
}
extern "C"
{
	__declspec(dllexport) void RunApp();    // run engine loop
}
extern "C"
{
	__declspec(dllexport) void QuitApp();    // quit
}