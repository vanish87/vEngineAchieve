#include "D3DRenderBuffer.h"
#include "Engine/Header/Context.h"
#include "D3DRenderEngine.h"

namespace vEngine
{
	D3DRenderBuffer::D3DRenderBuffer(void)
	{
	}

	D3DRenderBuffer::D3DRenderBuffer( void* init_data, BufferUsage buffer_usage, AccessType access_type )
		: RenderBuffer(buffer_usage, access_type)
	{
		d3d_buffer_ = (ID3D11Buffer*)init_data;
		if(usage_ == BU_SHADER_RES || usage_ == BU_SR_SB)
		{
			D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
			//optional
			//D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
			//sr_desc.Format = TEXTURE2D;
			if(FAILED(d3d_re->D3DDevice()->CreateShaderResourceView(d3d_buffer_, 0, &d3d_shader_resource_view_)))
				PRINT_ERROR("Cannot create Shader Resource View");
		}
		else
		if(usage_ == BU_SHADER_CONST)
		{
			PRINT_AND_BREAK("Not implemented");
		}
	}


	D3DRenderBuffer::~D3DRenderBuffer(void)
	{
	}

	void D3DRenderBuffer::UnMap()
	{
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		d3d_re->D3DDeviceImmContext()->Unmap(d3d_buffer_, 0);

	}

	void D3DRenderBuffer::DoMap( AccessType access_type )
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		//TODO : According to access_type, decide D3D_MAP_TYPE
		HRESULT result =  d3d_re->D3DDeviceImmContext()->Map(d3d_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(result))
			PRINT_ERROR("Failed to map resource");
		data_ = mappedResource.pData;
	}

	/*void D3DShaderResourceView::UnMap()
	{
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		d3d_re->D3DDeviceImmContext()->Unmap(d3d_texture_->D3DTexture(), 0);
	}

	void D3DShaderResourceView::DoMap( AccessType access_type )
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		//TODO : According to access_type, decide D3D_MAP_TYPE
		HRESULT result = d3d_re->D3DDeviceImmContext()->Map(d3d_texture_->D3DTexture(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			PRINT_AND_BREAK("Failed to map resource");
		data_ = mappedResource.pData;
	}

	D3DShaderResourceView::D3DShaderResourceView( Texture& texture, int array_size, int mip_level )
	{
		d3d_texture_= static_cast<D3DTexture2D*>(&texture);
		shader_resource_view_ = d3d_texture_->GetShaderResourceView(array_size, mip_level, texture.GetType());
	}*/

}
