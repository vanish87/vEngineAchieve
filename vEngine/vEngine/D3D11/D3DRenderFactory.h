#ifndef D3DRENDERFACTORY_H_
#define D3DRENDERFACTORY_H_

#pragma once

#include "Engine/Header/EnginePreDec.h"
#include "Engine/Header/Context.h"
#include "Engine/Header/RenderFactory.h"
#include "D3DPreDec.h"

#include "D3DRenderEngine.h"
#include "D3DFrameBuffer.h"
#include "D3DRenderEngine.h"
#include "D3DRenderLayout.h"
#include "D3DRenderBuffer.h"
#include "D3DTexture.h"

namespace vEngine
{
	class VENGINE_API D3DRenderFactory : public RenderFactory
	{
	public:
		D3DRenderFactory(void);
		~D3DRenderFactory(void);		
		
		virtual FrameBuffer* MakeFrameBuffer(uint32_t width, uint32_t height);

	private:

		virtual RenderEngine* CreateRenderEngine();

		virtual RenderLayout* MakeRenderLayout();

		RenderBuffer* MakeRenderBuffer(InitData& init_data, AccessType access_type, BufferUsage usage, uint32_t width, uint32_t type_size);

		//virtual RenderBuffer* MakeRenderBuffer( Texture* texture, AccessType access_type, BufferUsage usage );

		virtual Texture* MakeTexture2D( InitData const * init_data, uint32_t width, uint32_t height, uint32_t numMipMaps, uint32_t array_size, 
			Format format, uint32_t sample_count, uint32_t sample_quality, AccessType access_type, TextureUsage texture_usage);

		virtual Texture* MakeTexture2D(void* TextureData);

		virtual ShaderObject* MakeShaderObject();

		//virtual RenderView* MakeRenderView(Texture* texture, int array_size, int mip_level);
		//virtual RenderView* MakeRenderView(Texture* texture, int array_size, int mip_level, DepthStencilUsage usage);


	};

}

#endif