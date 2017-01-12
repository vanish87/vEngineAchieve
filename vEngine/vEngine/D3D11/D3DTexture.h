#ifndef D3DTEXTURE_H_
#define D3DTEXTURE_H_

#pragma once

#include "EnginePreDec.h"
#include "Texture.h"
#include "D3DPreDec.h"

namespace MocapGE
{
	class D3DTexture2D :	public Texture
	{
	public:
		D3DTexture2D(void);
		~D3DTexture2D(void);

		D3DTexture2D(uint32_t width, uint32_t height, AccessType access_type, uint32_t array_size, 
			Format format, uint32_t mip_level, uint32_t sample_count, uint32_t sample_quality, TextureUsage usage,
			InitData const * init_data);
		D3DTexture2D(D3D11_TEXTURE2D_DESC desc,	ID3D11Texture2D* & texture, TextureType type);

		ID3D11Texture2D* const & D3DTexture() const
		{
			return d3d_texture2D_;
		}

		ID3D11RenderTargetView* GetRenderTargetView( int array_size, int mip_level , TextureType type);
		ID3D11ShaderResourceView* GetShaderResourceView( int array_size, int mip_level , TextureType type);
		size_t GetWidth();
		size_t GetHeight();
		Format GetFormat();
		void SetD3DTexture( ID3D11Texture2D* texture );
	private:
		D3D11_TEXTURE2D_DESC desc_;
		ID3D11Texture2D* d3d_texture2D_;

		ID3D11RenderTargetView*   d3d_rt_view_;
		ID3D11ShaderResourceView* d3d_sr_view_;
		ID3D11DepthStencilView*   d3d_ds_view_;

		size_t	width_;
		size_t	height_;
	};
}

#endif

