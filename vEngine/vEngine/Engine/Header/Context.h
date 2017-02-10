#ifndef CONTEXT_H_
#define CONTEXT_H_

#pragma once

#include <string>

#include "Common\Header\CommonPreDec.h"
#include "Common\Header\Configure.h"
#include "Engine\Header\RenderFactory.h"
#include "Engine\Header\SceneManager.h"
#include "Engine\Header\StateManager.h"
#include "Engine\Header\EnginePreDec.h"

namespace vEngine
{
	class Context
	{
	public:

		static Context& Instance();
		
		void Setup(Configure::ContextSetting const & cfg);
		Configure::ContextSetting const & GetConfigure() const;

		RenderFactory& GetRenderFactory();
		SceneManager&  GetSceneManager();
		StateManager&  GetStateManager();
		ScriptContext& GetScriptContext();
		void RegisterAppInstance(AppSharedPtr app_instance);
		const App& GetAppInstance() const;

	private:
		SINGLETON_PRAVATE(Context)

		Configure::ContextSetting context_config_;

		RenderFactorySharedPtr	render_factory_;
		SceneManager scene_manager_;
		StateManager state_manager_;
		ScriptContextSharedPtr script_context_;

		AppSharedPtr app_instance_;
	};
}

#endif

