#ifndef DEFERREDRENDERING_H_
#define DEFERREDRENDERING_H_

#pragma once

#include <vector>

#include "EnginePreDec.h"
#include "Engine\Header\Context.h"
#include "FrameBuffer.h"
#include "RenderLayout.h"
#include "Mesh.h"

#include "PostProcess.h"

namespace vEngine
{
	//temp solution for pp
	class D3DShaderobject;
	class D3DModel;

	class DeferredRendering
	{
	public:
		DeferredRendering(const Configure::RenderSetting& render_setting);
		~DeferredRendering(void);
		
		FrameBuffer* & GetGBuffer(){return gbuffer_;};
		//std::vector<RenderBuffer*> GetGBufferSRV(){return gbuffer_srv_;};

		//Mesh* GetQuadMesh(){return fullscreen_mesh_;};
		//Mesh* GetFullscreenQuad();

		//void AddLightingBuffer( RenderBuffer* shader_resource);

		FrameBuffer* & GetLighingBuffer(){return lighting_buffer_;};
		//RenderBuffer* & GetLightingBufferSRV(){return lighting_srv_;};

		void Update();


		//debug functions
		void OutputGBufferToFrame(FrameBuffer* GBuffer, uint32_t GBufferIndex, FrameBuffer* OutBuffer);

		void ToggleGbuffer(uint32_t gbuffer_index);
		void ToggleLighting();

	private:
		//for Render Target
		FrameBuffer* gbuffer_;
		//for Shader Resource
		//std::vector<RenderBuffer*> gbuffer_srv_;
		std::vector<Texture*> gbuffer_tex_;
		//for Lighting Buffer
		FrameBuffer* lighting_buffer_;
		Texture* lighting_tex_;
		//RenderBuffer* lighting_srv_;

		//depth buffer
		ShaderObject* linearize_depth_so_;
		PostProcess* linearize_depth_pp_;
		Texture* depth_tex_;
		Texture* linear_depth_tex_;

		//for shadowing
		//FrameBuffer* shadowing_buffer_;
		//RenderBuffer* shadowing_srv_;
		Texture* shadowing_tex_;

		//back camera and buffer
		FrameBuffer* back_buffer_;
		Camera* main_camera_; 

		Texture* shadow_blur_;

		ShaderObject* shadow_map_blur_so_;
		PostProcess* shadow_map_xblur_pp_;
		PostProcess* shadow_map_yblur_pp_;
		Texture* shadow_blur_X_;
		Texture* shadow_blur_Y_;

		ShaderObject* linearize_shadow_map_so_;
		PostProcess* linearize_shadow_map_pp_;
		FrameBuffer* shadow_map_buffer_;
		Texture* shadow_depth_;
		Texture* shadow_linear_depth_;

		//final blur texture
		//RenderBuffer* shadow_blur_srv_;

		//RenderBuffer* depth_srv_;

		ShaderObject* ssdo_so_;
		PostProcess* ssdo_pp_;
		ShaderObject* occlusion_blur_so_;
		PostProcess* occlusion_xblur_pp_;
		PostProcess* occlusion_yblur_pp_;
 		Texture* occlusion_tex_;
		Texture* occlusion_blur_X_;
 		Texture* occlusion_blur_tex_;
		Texture* noise_tex_;
		//RenderBuffer* occlusion_blur_srv_;


		Mesh* fullscreen_mesh_;

		//debug
		PostProcess* output_to_tex_pp_;
		ShaderObject* output_to_tex_so_;

	};
}

#endif

