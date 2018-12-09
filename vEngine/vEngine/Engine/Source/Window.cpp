#include "Engine/Header/Window.h"
#include "Engine/Header/GameState.h"

void WarningText(std::string text)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	/* Save current attributes */
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	std::cout << "WARNING: " << text << std::endl;
	//printf("This is some nice COLORFUL text, isn't it?");
	/* Restore original attributes */
	SetConsoleTextAttribute(hConsole, saved_attributes);
	//printf("Back to normal");
};

void ErrorText(std::string text)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	/* Save current attributes */
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	std::cout << "ERROR: " << text << std::endl;
	//printf("This is some nice COLORFUL text, isn't it?");
	/* Restore original attributes */
	SetConsoleTextAttribute(hConsole, saved_attributes);
	//printf("Back to normal");

	__debugbreak();
};

namespace vEngine
{
	Window::Window(void)
		:inited_(false)
	{
	}

	void Window::InitWindow( std::string app_name, Configure::RenderSetting& render_setting, void* hwnd)
	{
		if (this->inited_)
			PRINT_AND_RETURN("Window has been Initied", );

		HRESULT Res = S_OK;
		if (hwnd != nullptr)
		{
			this->wnd_ = static_cast<HWND>(hwnd);
			this->default_wnd_proc_ = reinterpret_cast<WNDPROC>(::GetWindowLongPtrW(wnd_, GWLP_WNDPROC));
			::SetWindowLongPtr(this->wnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
			::SetWindowLongPtr(this->wnd_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));
		}
		else
		{

			this->win_name_ = app_name;
			std::wstring win_name_w = std::wstring(win_name_.begin(), win_name_.end());

			HINSTANCE hInstance = ::GetModuleHandle(nullptr);


			WNDCLASS wcex;
			wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;//CS_OWNDC to create own device context
			wcex.lpfnWndProc = WndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = sizeof(this);
			wcex.hInstance = hInstance;
			wcex.hIcon = nullptr;
			wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wcex.lpszMenuName = nullptr;
			wcex.lpszClassName = win_name_w.c_str();

			RegisterClass(&wcex);
			Res = GetLastError();
			CHECK_AND_ASSERT(Res == S_OK, "Register Window Failed");

			RECT rc = { 0, 0, render_setting.width, render_setting.height };
			//get real window size; should slightly bigger than rendering resolution
			//we should render a frame with render_setting, so window is enlarged.
			::AdjustWindowRect(&rc, render_setting.full_screen ? WS_POPUP : WS_OVERLAPPEDWINDOW, FALSE);

			window_rect_.top = static_cast<uint16_t>(render_setting.top);
			window_rect_.left = static_cast<uint16_t>(render_setting.left);
			window_rect_.width = static_cast<uint16_t>(rc.right - rc.left);
			window_rect_.height = static_cast<uint16_t>(rc.bottom - rc.top);
			this->wnd_ = CreateWindow(win_name_w.c_str(), win_name_w.c_str(),
				WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
				window_rect_.left, window_rect_.top, window_rect_.width, window_rect_.height,
				nullptr, nullptr, hInstance, nullptr);		

			if (!this->wnd_)
			{
				Res = GetLastError();
				CHECK_ASSERT(false);
				PRINT_AND_RETURN("Create Window Failed", );
			}

			::SetWindowLongPtr(this->wnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

			::ShowWindow(this->wnd_, SW_SHOWNORMAL);
			::SetForegroundWindow(this->wnd_);
			::SetFocus(this->wnd_);
			::ShowCursor(!render_setting.full_screen);
			::UpdateWindow(this->wnd_);

			this->default_wnd_proc_ = ::DefWindowProc;
		}

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
			LRESULT ret = win->MsgProc(hWnd, message, wParam, lParam);
			CHECK_ASSERT(ret == S_OK);
			return win->default_wnd_proc_(hWnd, message, wParam, lParam);
		}
		
		return S_FALSE;
	}

	LRESULT CALLBACK Window::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_QUIT:
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			if (Context::Instance().GetStateManager().CurrentState() != nullptr)
				Context::Instance().GetStateManager().CurrentState()->OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			if (Context::Instance().GetStateManager().CurrentState() != nullptr)
				Context::Instance().GetStateManager().CurrentState()->OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_MOUSEMOVE:
			if (Context::Instance().GetStateManager().CurrentState() != nullptr)
				Context::Instance().GetStateManager().CurrentState()->OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_KEYDOWN:
			if (Context::Instance().GetStateManager().CurrentState() != nullptr)
				Context::Instance().GetStateManager().CurrentState()->OnKeyDown(wParam);
			break;
		case WM_KEYUP:
			if (Context::Instance().GetStateManager().CurrentState() != nullptr)
				Context::Instance().GetStateManager().CurrentState()->OnKeyUp(wParam);
			break;
			//case WM_PAINT
			//unreal calls rendering function here
		default:
			break;
		}

		return S_OK;
	}

	int2 Window::GetCenter()
	{
		RECT rect;
		::GetClientRect(wnd_, &rect);
		int2 ret(rect.left + rect.right /2, rect.top + rect.bottom /2);
		return ret;
	}

}
