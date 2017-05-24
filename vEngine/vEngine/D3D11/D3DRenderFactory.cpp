#include "D3DRenderFactory.h"

namespace vEngine
{
	D3DRenderFactory::D3DRenderFactory(void)		
	{
		render_engine_ = nullptr;
	}


	D3DRenderFactory::~D3DRenderFactory(void)
	{
	}

	RenderEngine* D3DRenderFactory::CreateRenderEngine()
	{
		return new D3DRenderEngine();
	}
	
	FrameBuffer* D3DRenderFactory::MakeFrameBuffer(uint32_t width, uint32_t height)
	{
		return new D3DFrameBuffer(width, height);
	}

	RenderLayout* D3DRenderFactory::MakeRenderLayout()
	{
		return new D3DRenderLayout();
	}

	RenderBuffer* D3DRenderFactory::MakeRenderBuffer( InitData& init_data, AccessType access_type, BufferUsage usage, uint32_t width, uint32_t type_size)
	{
		//TODO : Change BufferUsage to support OR operation
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		D3D11_BUFFER_DESC buffer_desc;
		buffer_desc.ByteWidth = type_size * width;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = 0;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = init_data.data;
		data.SysMemPitch = init_data.row_pitch;
		data.SysMemSlicePitch = init_data.slice_pitch;
		switch (usage)
		{
		case BU_VERTEX:
			{
				buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				break;
			}
		case BU_INDEX:
			{				
				buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				break;
			}
		case BU_SHADER_RES:
			{
				buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				break;
			}
		case BU_SHADER_CONST:
			break;
		case  BU_STRUCTURED_BUFFER:
			break;
		case BU_SR_SB:
			{
				buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				buffer_desc.StructureByteStride = type_size;
			}
			break;
		default:
			break;
		}

		switch (access_type)
		{
		case AT_CPU_GPU_ALL:
			buffer_desc.Usage = D3D11_USAGE_STAGING;
			buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			break;
		case AT_CPU_WRITE_GPU_READ:
			buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
			buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		case AT_GPU_READ_ONLY:
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.CPUAccessFlags = 0;
			break;
		case AT_GPU_READ_WRITE:
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.CPUAccessFlags = 0;
			break;
		default:
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.CPUAccessFlags = 0;
			break;
		}
		ID3D11Buffer* buffer;
		HRESULT result = d3d_re->D3DDevice()->CreateBuffer(&buffer_desc, &data, &buffer);
		if(FAILED(result))
		{
			PRINT_ERROR("Cannot create Buffer");
		}
		D3DRenderBuffer* d3d_render_buffer = new D3DRenderBuffer(buffer, usage, access_type);
		return d3d_render_buffer;
		
	}

	/*RenderBuffer* D3DRenderFactory::MakeRenderBuffer( Texture* texture, AccessType access_type, BufferUsage usage )
	{
		//TODO init AccessType access_type, BufferUsage usage and other render buffer varibles
		return new D3DShaderResourceView(*texture, 1, 1);
	}*/

	Texture* D3DRenderFactory::MakeTexture2D( InitData const * init_data, uint32_t width, uint32_t height, uint32_t numMipMaps, uint32_t array_size, Format format, uint32_t sample_count, uint32_t sample_quality, AccessType access_type, TextureUsage texture_usage )
	{		
		return new D3DTexture2D(width, height, access_type, array_size, 
			format, numMipMaps, sample_count, sample_quality, texture_usage,
			init_data);
	}

	Texture* D3DRenderFactory::MakeTexture2D(void* TextureData)
	{
		CHECK_ASSERT(TextureData != nullptr);
		ID3D11Texture2D* ptr = static_cast<ID3D11Texture2D*>(TextureData);
		return new D3DTexture2D(ptr);
	}

	ShaderObject* D3DRenderFactory::MakeShaderObject()
	{
		return new D3DShaderobject();
	}

	/*RenderView* D3DRenderFactory::MakeRenderView(Texture* texture, int array_size, int mip_level)
	{
		return new D3DRenderTargetView(*texture, array_size, mip_level );
	}

	RenderView* D3DRenderFactory::MakeRenderView(Texture* texture, int array_size, int mip_level, DepthStencilUsage usage)
	{
		return new D3DDepthStencilRenderView(*texture, array_size, mip_level, usage);
	}*/

}