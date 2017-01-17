#ifndef WINDOW_H_
#define WINDOW_H_

#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <string>

#include "Common\Header\CommonPreDec.h"
#include "Common\Header\Configure.h"
#include "Engine\Header\Context.h"
#include "App.h"

namespace MocapGE
{
	class Window
	{
	public:
		Window(std::string app_name, Configure::RenderSetting render_setting);
		~Window(void);

		HWND GetWnd() const
		{
			return wnd_;
		}

		int2 GetCenter();

	private:
		Window(void);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK MsgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
		HWND wnd_;
		RECT rect;
		std::string win_name_;
	};
}


#endif

