#ifndef D3DTEXTURE_H_
#define D3DTEXTURE_H_

#pragma once

#include "Engine\Header\EnginePreDec.h"
#include "Engine\Header\Texture.h"
#include "D3DPreDec.h"

namespace vEngine
{
	class D3DTexture2D :	public Texture
	{
	public:
		D3DTexture2D(void);
		~D3DTexture2D(void);

		D3DTexture2D(uint32_t width, uint32_t height, AccessType access_type, uint32_t array_size, 
			Format format, uint32_t mip_level, uint32_t sample_count, uint32_t sample_quality, TextureUsage usage,
			InitData const * init_data);
		D3DTexture2D(ID3D11Texture2D* & texture);

		ID3D11RenderTargetView* GetRenderTargetView(int array_size, int mip_level, TextureType type);
		ID3D11ShaderResourceView* GetShaderResourceView(int array_size, int mip_level, TextureType type);
		ID3D11DepthStencilView* GetDepthStencilView(int array_size, int mip_level, TextureType type);
		uint32_t GetWidth();
		uint32_t GetHeight();
		Format GetFormat();
		void SetD3DTexture( ID3D11Texture2D* texture );

	protected:

		virtual void DoMap(AccessType access_type);
		virtual void UnMap();
	private:
		D3D11_TEXTURE2D_DESC desc_;
		ID3D11Texture2D* d3d_texture2D_;

		ID3D11RenderTargetView*   d3d_rt_view_;
		ID3D11ShaderResourceView* d3d_sr_view_;
		ID3D11DepthStencilView*   d3d_ds_view_;

		uint32_t	width_;
		uint32_t	height_;
	};
}

#endif

