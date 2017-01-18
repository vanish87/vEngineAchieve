#include "Engine\Header\Window.h"

namespace vEngine
{
	Window::Window(void)
	{
	}

	Window::Window( std::string app_name, Configure::RenderSetting render_setting )
		:win_name_(app_name)
	{
		HINSTANCE hInstance = ::GetModuleHandle(NULL);

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof( WNDCLASSEX );
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(this);
		wcex.hInstance = hInstance;
		wcex.hIcon = NULL;
		wcex.hCursor = ::LoadCursor( NULL, IDC_ARROW );
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = win_name_.c_str();
		wcex.hIconSm = NULL;
		if( !RegisterClassEx( &wcex ) )
			PRINT("Register Window Failed");

		uint32_t wstyle = render_setting.full_screen ? WS_POPUP : WS_OVERLAPPEDWINDOW;

		RECT rc = { render_setting.left, render_setting.top, render_setting.width, render_setting.height };
		::AdjustWindowRect( &rc, wstyle, FALSE );
		wnd_ = CreateWindow( win_name_.c_str(), win_name_.c_str(), wstyle,
			render_setting.left, render_setting.top, rc.right - rc.left, rc.bottom - rc.top, 
			NULL, NULL, hInstance, NULL );

		if( !wnd_ )
			PRINT("Create Window Failed");

		::SetWindowLongPtr(wnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		
		::ShowWindow(wnd_, SW_SHOWNORMAL);
		::SetForegroundWindow(wnd_);
		::SetFocus(wnd_);
		::ShowCursor(!render_setting.full_screen);
		::UpdateWindow(wnd_);
	}


	Window::~Window(void)
	{
		if (wnd_ != NULL)
		{
			::DestroyWindow(wnd_);
			wnd_ = NULL;
		}
	}

	LRESULT CALLBACK Window::WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
	{
		Window* win = reinterpret_cast<Window*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));

		if (win != nullptr)
		{
			return win->MsgProc(hWnd, message, wParam, lParam);
		}
		else
		{
			return ::DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	LRESULT CALLBACK Window::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch( message )
		{
		case WM_QUIT:
		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			if(Context::Instance().GetStateManager().CurrentState() != nullptr)
				Context::Instance().GetStateManager().CurrentState()->OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			if(Context::Instance().GetStateManager().CurrentState() != nullptr)
				Context::Instance().GetStateManager().CurrentState()->OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_MOUSEMOVE:
			if(Context::Instance().GetStateManager().CurrentState() != nullptr)
				Context::Instance().GetStateManager().CurrentState()->OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_KEYDOWN: 
			if(Context::Instance().GetStateManager().CurrentState() != nullptr)
				Context::Instance().GetStateManager().CurrentState()->OnKeyDown(wParam);
			break;
		case WM_KEYUP: 
			if(Context::Instance().GetStateManager().CurrentState() != nullptr)
				Context::Instance().GetStateManager().CurrentState()->OnKeyUp(wParam);
			break;
		//case WM_PAINT
		//unreal calls rendering function here

		default:
			{
				return ::DefWindowProc(hWnd, message, wParam, lParam);
			}

		}
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	int2 Window::GetCenter()
	{
		::GetClientRect(wnd_, &rect);
		int2 ret(rect.left + rect.right /2, rect.top + rect.bottom /2);
		return ret;
	}

}
