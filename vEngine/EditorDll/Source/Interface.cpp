#include "Header/Interface.h"
#include "Common/Header/CommonPreDec.h"
#include "Engine/Header/EnginePreDec.h"
#include "Common/Header/Vector.h"
#include "Common/Header/Math.h"

using namespace vEngine;

void InitD3D(HWND hWnd, int Width, int Height)
{
	if (!hWnd)
	{
		HRESULT Res = GetLastError();
		CHECK_ASSERT(false);
		PRINT_AND_RETURN("Create Window Failed", );
	}

	::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(hWnd));

	::ShowWindow(hWnd, SW_SHOWNORMAL);
	::SetForegroundWindow(hWnd);
	::SetFocus(hWnd);
	//::ShowCursor(!render_setting.full_screen);
	::UpdateWindow(hWnd);

	float4 test;
	test = test * 3;
}
void RenderFrame(void)
{
	MessageBox(0, L"Hello World from DLL!\n", L"Hi", MB_ICONINFORMATION);
}

void CleanD3D(void)
{

}
