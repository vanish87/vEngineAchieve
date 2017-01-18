#include "D3DTexture.h"
#include "Engine\Header\Context.h"
#include "D3DRenderEngine.h"
#include "Engine\Header\RenderView.h"

namespace vEngine
{
	D3DTexture2D::D3DTexture2D(void)
		:d3d_texture2D_(nullptr),d3d_rt_view_(nullptr), d3d_sr_view_(nullptr), d3d_ds_view_(nullptr)
	{
		type_ = TEXTURE2D;
	}

	D3DTexture2D::D3DTexture2D( uint32_t width, uint32_t height, AccessType access_type, uint32_t array_size, 
								Format format, uint32_t mip_level, uint32_t sample_count, uint32_t sample_quality, TextureUsage usage,
								InitData const * init_data )
		:Texture(access_type, array_size, format, mip_level, sample_count, sample_quality, usage, TEXTURE2D)
		,width_(width), height_(height), d3d_rt_view_(nullptr), d3d_sr_view_(nullptr), d3d_ds_view_(nullptr)
	{

		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		desc_.Width = (UINT)width_;
		desc_.Height = (UINT)height_;
		desc_.ArraySize = array_size_;
		desc_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc_.CPUAccessFlags = 0;
		desc_.Format = d3d_re->MapFormat(format_);
		desc_.MipLevels = mip_level_;
		desc_.MiscFlags = (mip_level_ != 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);
		desc_.SampleDesc.Quality = sample_quality;
		desc_.SampleDesc.Count = sample_count;
		desc_.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA d3d_init_data;
		switch (access_type_)
		{
		case AT_CPU_READ:
			desc_.Usage = D3D11_USAGE_DYNAMIC;
			desc_.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			break;
		case AT_CPU_WRITE:
			desc_.Usage = D3D11_USAGE_DYNAMIC;
			desc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		case AT_GPU_READ:
			break;
		case AT_GPU_WRITE:
			break;
		default:
			break;
		}
		switch (usage_)
		{
		case TU_SHADER_RES:
			desc_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			break;
		case TU_RENDER_TARGET:
			desc_.BindFlags = D3D11_BIND_RENDER_TARGET;
			break;
		case TU_SHADER_CONST:
			desc_.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			break;
		case TU_STRUCTURED_BUFFER:
			desc_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			break;
		case TU_SR_RT:
			desc_.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			break;
		default:
			break;
		}
		if(init_data)
		{
			d3d_init_data.pSysMem = init_data->data;
			d3d_init_data.SysMemPitch =  init_data->row_pitch;
			d3d_init_data.SysMemSlicePitch = init_data->slice_pitch;
		}
		HRESULT result = d3d_re->D3DDevice()->CreateTexture2D(&desc_, init_data==nullptr? 0 :&d3d_init_data, &d3d_texture2D_);
		if(FAILED(result))
			PRINT("Cannot create Texture 2D");
	}

	D3DTexture2D::D3DTexture2D( D3D11_TEXTURE2D_DESC desc, ID3D11Texture2D* & texture , TextureType type)
		:d3d_rt_view_(nullptr), d3d_sr_view_(nullptr), d3d_ds_view_(nullptr)
	{
		//TODO : Use desc to Init
		desc_ = desc;
		usage_ = TU_SHADER_RES;
		format_ = R8G8B8A8_U;
		d3d_texture2D_ = texture;
		type_ = type;
	}


	D3DTexture2D::~D3DTexture2D(void)
	{
	}

	ID3D11RenderTargetView* D3DTexture2D::GetRenderTargetView( int array_size, int mip_level , TextureType type)
	{
		if(d3d_rt_view_ == nullptr)
		{
			//check it first
			D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			assert(array_size == 1);
			if(array_size == 1)
			{
				desc.Format = d3d_re->MapFormat(this->format_);
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = mip_level;
			}
			HRESULT result = d3d_re->D3DDevice()->CreateRenderTargetView(this->D3DTexture(), &desc, &d3d_rt_view_);
			if(FAILED(result))
				PRINT("Cannot create D3DRenderTargetView");
		}
		return d3d_rt_view_;
	}

	size_t D3DTexture2D::GetWidth()
	{
		return width_;
	}

	size_t D3DTexture2D::GetHeight()
	{
		return height_;
	}

	Format D3DTexture2D::GetFormat()
	{
		return format_;
	}

	void D3DTexture2D::SetD3DTexture( ID3D11Texture2D* d3d_texture )
	{
		this->d3d_texture2D_ = d3d_texture;
	}

	ID3D11ShaderResourceView* D3DTexture2D::GetShaderResourceView( int array_size, int mip_level , TextureType type)
	{
		if( d3d_sr_view_== nullptr)
		{
			D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
			D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
			switch (type)
			{
			case TEXTURE1D:
				break;
			case TEXTURE2D:
				sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				sr_desc.Texture2D.MipLevels = mip_level;
				sr_desc.Texture2D.MostDetailedMip = 0;
				break;
			case TEXTURECUBE:
				sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				sr_desc.TextureCube.MipLevels = mip_level;
				sr_desc.TextureCube.MostDetailedMip = 0;
				
				break;
			default:
				break;
			}
			
			//TODO: init format
			//sr_desc.Format = d3d_re->MapFormat(format_);
			sr_desc.Format = desc_.Format;
			switch (usage_)
			{
			case TU_DEPTH_SR:
				sr_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			default:
				break;
			}
			
			if(FAILED(d3d_re->D3DDevice()->CreateShaderResourceView(this->D3DTexture(), &sr_desc, &d3d_sr_view_)))
				PRINT("Cannot create Shader Resource View");
		}
		return d3d_sr_view_;
	}


}
