#include "Engine\Header\RenderFactory.h"

namespace vEngine
{
	RenderFactory::RenderFactory(void)
	{
	}


	RenderFactory::~RenderFactory(void)
	{
	}

	RenderEngine& RenderFactory::GetRenderEngine()
	{
		if(render_engine_ == nullptr)
			render_engine_ = this->CreateRenderEngine();

		return *render_engine_;
	}


}