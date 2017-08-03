#include "Engine/Header/RenderEngine.h"

namespace vEngine
{
	RenderEngine::RenderEngine(void)
	{
	}


	RenderEngine::~RenderEngine(void)
	{
	}

	void RenderEngine::InitRenderWindow(const std::string& name, const Configure::RenderSetting& render_setting )
	{
		name_ = name;
		render_setting_ = render_setting;
	}
	
	void RenderEngine::InitDeferredRendering( const Configure::RenderSetting& render_setting )
	{
		deferred_rendering_ = new DeferredRendering(render_setting);
	}
}
