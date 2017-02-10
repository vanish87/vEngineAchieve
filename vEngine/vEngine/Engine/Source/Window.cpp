#include "Engine\Header\Window.h"
#include "Engine\Header\GameState.h"

namespace vEngine
{
	Window::Window(void)
		:inited_(false)
	{
	}

	void Window::InitWindow( std::string app_name, Configure::RenderSetting& render_setting )
	{
		if (this->inited_)
			PRINT_AND_RETURN("Window has been Initied", );

		this->win_name_ = app_name;

		HINSTANCE hInstance = ::GetModuleHandle(NULL);

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof( WNDCLASSEX );
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(this);
		wcex.hInstance = hInstance;
		wcex.hIcon = nullptr;
		wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW );
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = win_name_.c_str();
		wcex.hIconSm = nullptr;
		if( !RegisterClassEx( &wcex ) )
			PRINT_AND_RETURN("Register Window Failed",);

		RECT rc = { 0, 0, render_setting.width, render_setting.height };
		//get real window size; should slightly bigger than rendering resolution
		//we should render a frame with render_setting, so window is enlarged.
		::AdjustWindowRect( &rc, render_setting.full_screen ? WS_POPUP : WS_OVERLAPPEDWINDOW, FALSE );

		window_rect_.top	= static_cast<uint16_t>(render_setting.top);
		window_rect_.left	= static_cast<uint16_t>(render_setting.left);
		window_rect_.width  = static_cast<uint16_t>(rc.right - rc.left);
		window_rect_.height = static_cast<uint16_t>(rc.bottom - rc.top);
		this->wnd_ = CreateWindow( win_name_.c_str(), win_name_.c_str(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
							window_rect_.left, window_rect_.top, window_rect_.width, window_rect_.height,
							nullptr, nullptr, hInstance, nullptr);

		if( !this->wnd_ )
			PRINT_AND_RETURN("Create Window Failed",);

		::SetWindowLongPtr(this->wnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		
		::ShowWindow(this->wnd_, SW_SHOWNORMAL);
		::SetForegroundWindow(this->wnd_);
		::SetFocus(this->wnd_);
		::ShowCursor(!render_setting.full_screen);
		::UpdateWindow(this->wnd_);

		this->inited_ = true;
	}


	Window::~Window(void)
	{
		if (this->wnd_ != nullptr)
		{
			::DestroyWindow(this->wnd_);
			this->wnd_ = nullptr;
		}
		this->inited_ = false;
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
		RECT rect;
		::GetClientRect(wnd_, &rect);
		int2 ret(rect.left + rect.right /2, rect.top + rect.bottom /2);
		return ret;
	}

}
