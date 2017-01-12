#include "Engine\Header\Context.h"
#include "D3D11\D3DRenderFactory.h"

namespace MocapGE
{
	Context::Context(void)
	{
	}


	Context::~Context(void)
	{
	}

	Context& Context::Instance()
	{
		static Context context;
		return context;
	}	

	Configure::ContextSetting const & Context::GetConfigure() const
	{
		return context_config_;
	}

	void Context::Setup(Configure::ContextSetting const & cfg )
	{
		if(!context_config_.render_factory_name.compare(cfg.render_factory_name) || render_factory_==nullptr)
		{
			//this->LoadRenderFactory(cfg.render_factory_name);
			render_factory_= new D3DRenderFactory();
		}

		//TODO: make it Singleton
		scene_manager_ = new SceneManager();
		state_manager_ = new StateManager();
		//resource_loader_ = new ResourceLoader();

		context_config_ = cfg;
	}

	RenderFactory& Context::GetRenderFactory()
	{
		if(render_factory_ == nullptr)std::cout<< "no Render Factory"<<std::endl;
		return *render_factory_;
	}

	SceneManager& Context::GetSceneManager()
	{
		if(scene_manager_ == nullptr)std::cout<< "no Scene Manager"<<std::endl;
		return *scene_manager_;
	}

	StateManager& Context::GetStateManager()
	{
		if(state_manager_ == nullptr)std::cout<< "no State Manager"<<std::endl;
		return *state_manager_;
	}

// 	ResourceLoader& Context::GetResourceLoader()
// 	{
// 		if(resource_loader_ == nullptr)std::cout<< "no Resource Loader"<<std::endl;
// 		return *resource_loader_;
// 	}

}

