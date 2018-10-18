#ifndef WINDOW_H_
#define WINDOW_H_

#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <string>

#include "Common/Header/CommonPreDec.h"
#include "Common/Header/Configure.h"
#include "Engine/Header/Context.h"

namespace vEngine
{
	class Window
	{
		friend class App;
	public:
		void InitWindow(std::string app_name, Configure::RenderSetting& render_setting, void* hwnd = nullptr);
		virtual ~Window(void);

		HWND GetWnd() const
		{
			return wnd_;
		}

		int2 GetCenter();
		struct WinndowRect
		{
			uint16_t top;
			uint16_t left;
			uint16_t width;
			uint16_t height;
		};

	private:
		Window(void);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK MsgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
		HWND wnd_;
		WNDPROC default_wnd_proc_;
		std::string win_name_;
		bool inited_; 
		//WinndowRect should slightly bigger than rendering resolution
		WinndowRect window_rect_;
	};
}


#endif

