#include "Engine/Header/App.h"
#include "Engine/Header/Context.h"
#include "Engine/Header/ScriptContext.h"
#include "Engine/Header/Window.h"

#include "Engine/Header/Font.h"
#include "Engine/Header/Text.h"

#include "Common/Header/Timer.h"


namespace vEngine
{
	App::App(void)
	{
	}

	App::App( std::string app_name )
		:app_name_(app_name), cursor_locked_(false)
	{

	}


	App::~App(void)
	{
	}

	void App::Initialize()
	{
		Configure::ContextSetting config = Context::Instance().GetConfigure();

		//register app here
		Context::Instance().RegisterAppInstance(this);

		//then create system window
		this->win_.InitWindow(app_name_, config.render_setting);

		//then create rendering window
		Context::Instance().GetRenderFactory().GetRenderEngine().InitRenderWindow(app_name_, config.render_setting);

		//init deferred rendering here
		if(config.render_setting.deferred_rendering)
			Context::Instance().GetRenderFactory().GetRenderEngine().InitDeferredRendering(config.render_setting);

		this->InitObjects();

	}

	void App::Run()
	{
		MSG msg = {0};
		//DWORD next_game_tick;
		//int loops;
		//next_game_tick = GetTickCount();

		Timer UpdateTimer;
		double Previous = UpdateTimer.Time();
		double Lag = 0;

		while( WM_QUIT != msg.message )
		{
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				//loops = 0;
				//while( GetTickCount() > next_game_tick && loops < MAX_CYCLES_PER_FRAME ) 

				double Current = UpdateTimer.Time();
				double Elapsed = Current - Previous;
				Previous = Current;
				Lag += Elapsed;
				//while(Lag >= MS_PER_UPDATE)
				{
					Font::GetFontByName("msyh").ClearCPUBuffer();
					this->Update();//do user's update

					Context::Instance().GetStateManager().Update();
					Context::Instance().GetSceneManager().Update();


// 					next_game_tick += UPDATE_INTERVAL;
// 					loops++;
					Lag -= MS_PER_UPDATE;
				}
				Context::Instance().GetSceneManager().Flush();

				if(cursor_locked_)
				{
					Configure::ContextSetting config = Context::Instance().GetConfigure();
					POINT pt; 
					pt.x = config.render_setting.width / 2;
					pt.y = config.render_setting.height/ 2;
					ClientToScreen(win_.GetWnd(), &pt); 
					::SetCursorPos(pt.x, pt.y);
				}

			}
		}

		//TODO: Shutdown all threads now
		Context::Instance().GetScriptContext().Quit();
	}

	const Window& App::GetWindow() const
	{
		return this->win_;
	}

	void App::LockCursor( bool lock )
	{
		cursor_locked_ = lock;
	}

	void App::SetCursor( int x, int y )
	{
		::SetCursorPos(x, y);
	}

}
