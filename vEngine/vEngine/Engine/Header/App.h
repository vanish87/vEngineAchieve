#ifndef APP_H_
#define APP_H_

#pragma once

#include <string>

#include "Engine\Header\EnginePreDec.h"
#include "Common\Header\Configure.h"
#include "Engine\Header\Window.h"

namespace vEngine
{
	class App
	{
	public:
		App(void);
		~App(void);
		App(std::string app_name);
		void Initialize();
		void Run();

		void LockCursor(bool lock);
		void SetCursor(int x, int y);

		const Window& GetWindow() const;

	private:
		std::string app_name_;
		Window win_;
		bool cursor_locked_;

		static const int MAXIMUM_FRAME_RATE = 60;
		static const int UPDATE_INTERVAL = 1000 / MAXIMUM_FRAME_RATE;
		static const int MINIMUM_FRAME_RATE = 24;
		static const int MAX_CYCLES_PER_FRAME = MAXIMUM_FRAME_RATE / MINIMUM_FRAME_RATE ;
		
	protected:
		virtual void InitObjects()
		{
		}
		virtual void Update()
		{
		}
		virtual void ReleaseObjects()
		{
		}



	};
}
#endif

