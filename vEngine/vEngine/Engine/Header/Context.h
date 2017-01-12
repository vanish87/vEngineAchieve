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

namespace MocapGE
{
	class Context
	{
	public:
		//TODO : make constructor to be protected: because of singleton pattern
		Context(void);
		~Context(void);

		static Context& Instance();
		
		void Setup(Configure::ContextSetting const & cfg);
		Configure::ContextSetting const & GetConfigure() const;

		RenderFactory& GetRenderFactory();
		SceneManager& GetSceneManager();
		StateManager& GetStateManager();
		//ResourceLoader& GetResourceLoader();

	private:
		Configure::ContextSetting context_config_;

		RenderFactory*	render_factory_;
		SceneManager* scene_manager_;
		StateManager* state_manager_;
		//ResourceLoader* resource_loader_;
	};
}

#endif

