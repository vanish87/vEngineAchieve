#ifndef RENDERFACTORY_H_
#define RENDERFACTORY_H_

#pragma once
#include "Engine/Header/EnginePreDec.h"

#include "Engine/Header/RenderEngine.h"
#include "Engine/Header/TypeAndFormat.h"
#include "Engine/Header/Texture.h"

namespace vEngine
{
	class RenderFactory
	{
	public:
		RenderFactory(void);
		virtual ~RenderFactory(void);

		RenderEngine& GetRenderEngine();

		virtual FrameBuffer* MakeFrameBuffer(uint32_t width, uint32_t type_size) = 0;
		virtual RenderLayout* MakeRenderLayout() = 0;	
		//make sure you you can use the same type when you call Map method of RenderBuffer
		//Only support one type of BufferUsage
		virtual RenderBuffer* MakeRenderBuffer(InitData& init_data, AccessType access_type, BufferUsage usage, uint32_t width, uint32_t type_size) = 0;
		//virtual RenderBuffer* MakeRenderBuffer(Texture* texture, AccessType access_type, BufferUsage usage) = 0;
		virtual Texture* MakeTexture2D(InitData const * init_data, uint32_t width, uint32_t height, uint32_t numMipMaps, uint32_t array_size,
			Format format, uint32_t sample_count, uint32_t sample_quality, AccessType access_type, TextureUsage texture_usage) = 0;
		//a remap from platform dependent data
		//e.g: from D3D11Texture to vEngine texture
		virtual Texture* MakeTexture2D(void* TextureData) = 0;
		virtual ShaderObject* MakeShaderObject() = 0;
		//virtual RenderView* MakeRenderView(Texture* texture, int array_size, int mip_level) = 0;
		//virtual RenderView* MakeRenderView(Texture* texture, int array_size, int mip_level, DepthStencilUsage usage) = 0;

		
	protected:
		RenderEngine* render_engine_;

		virtual RenderEngine* CreateRenderEngine() = 0;
	};
}

#endif