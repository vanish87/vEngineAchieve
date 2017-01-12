#include "Engine\Header\RenderEngine.h"

namespace MocapGE
{
	RenderEngine::RenderEngine(void)
	{
	}


	RenderEngine::~RenderEngine(void)
	{
	}

	void RenderEngine::InitRenderWindow( std::string const & name, Configure::RenderSetting const & render_setting )
	{
		name_ = name;
		render_setting_ = render_setting;
	}

	void RenderEngine::InitLights()
	{
		std::vector<Light*> lights = Context::Instance().GetSceneManager().GetLights();
		size_t size = lights.size();
		if(size == 0)
		{
			lights.push_back(new PointLight());
			size = lights.size();
		}
		/*LightStruct* light = new LightStruct[size];
		for(size_t i = 0; i < size; i++)
		{
			light[i].color = lights[i]->GetColor();
			light[i].position = static_cast<PointLight*>(lights[i])->GetPos();
		}
		InitData init_data;
		init_data.data = light;
		init_data.row_pitch = 0;
		init_data.slice_pitch = 0;
		light_buffer_ = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_CPU_WRITE, BU_SR_SB, size , sizeof(LightStruct));*/
	}

	//TODO : move it to DeferredRendering class
	void RenderEngine::InitDeferredRendering( Configure::RenderSetting render_setting )
	{
		deferred_rendering_ = new DeferredRendering(render_setting);
	}

/*
	FrameBuffer* RenderEngine::GetGBuffer()
	{
		return deferred_rendering_->GetGBuffer();
	}
	std::vector<RenderBuffer*> RenderEngine::GetGBufferSRV()
	{
		return deferred_rendering_->GetGBufferSRV();
	}

	Mesh* RenderEngine::GetFullscreenQuad()
	{
		return deferred_rendering_->GetFullscreenQuad();
	}

	FrameBuffer* RenderEngine::GetLightingBuffer()
	{
		return deferred_rendering_->GetLighingBuffer();
	}

	RenderBuffer* RenderEngine::GetLightingBufferSRV()
	{
		return deferred_rendering_->GetLightingBufferSRV();
	}*/

}
