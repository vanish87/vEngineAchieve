#ifndef CONTEXT_H_
#define CONTEXT_H_

#pragma once

#include <string>

#include "Common\Header\CommonPreDec.h"
#include "Common\Header\Configure.h"
#include "RenderFactory.h"
#include "SceneManager.h"
#include "StateManager.h"
//#include "ResourceLoader.h" // find in Deprecated Files folder

namespace vEngine
{
	class Context
	{
	public:

		static Context& Instance();
		
		void Setup(Configure::ContextSetting const & cfg);
		Configure::ContextSetting const & GetConfigure() const;

		RenderFactory& GetRenderFactory();
		SceneManager& GetSceneManager();
		StateManager& GetStateManager();
		void RegisterAppInstance(App* app_instance);
		const App& GetAppInstance() const;
		//ResourceLoader& GetResourceLoader();

	private:
		SINGLETON_PRAVATE(Context)

		Configure::ContextSetting context_config_;

		RenderFactory*	render_factory_;
		SceneManager scene_manager_;
		StateManager state_manager_;
		//ResourceLoader* resource_loader_;

		App* app_instance_;
	};
}

#endif

