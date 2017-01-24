#include "Engine\Header\Context.h"
#include "D3D11\D3DRenderFactory.h"

namespace vEngine
{
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
		//this is supposed to choose rendering plugin by configure
		if(!context_config_.render_factory_name.compare(cfg.render_factory_name) || render_factory_==nullptr)
		{
			//this->LoadRenderFactory(cfg.render_factory_name);
			render_factory_= new D3DRenderFactory();
		}

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
		return scene_manager_;
	}

	StateManager& Context::GetStateManager()
	{
		return state_manager_;
	}

	void Context::RegisterAppInstance(App * app_instance)
	{
		if (this->app_instance_ != nullptr)
			PRINT_AND_ASSERT("this is not supported");
		
		this->app_instance_ = app_instance;
	}

	const App& Context::GetAppInstance() const
	{
		return *this->app_instance_;
	}

// 	ResourceLoader& Context::GetResourceLoader()
// 	{
// 		if(resource_loader_ == nullptr)std::cout<< "no Resource Loader"<<std::endl;
// 		return *resource_loader_;
// 	}

}

