#include "Engine\Header\Texture.h"

namespace vEngine
{
	Texture::Texture(void)
	{
	}

	Texture::Texture( AccessType access_type, uint32_t array_size, Format format, uint32_t mip_level, uint32_t sample_count, uint32_t sample_quality, TextureUsage usage, TextureType type )
		: access_type_(access_type), array_size_(array_size), format_(format), mip_level_(mip_level), sample_count_(sample_count),sample_quality_(sample_quality),
		usage_(usage), type_(type)
	{

	}


	Texture::~Texture(void)
	{
	}

	void Texture::SetUsage( TextureUsage usage )
	{
		usage_ = usage;
	}

	TextureType Texture::GetType()
	{
		return type_;
	}


	NTexture::NTexture()
		:Texture(AT_GPU_READ_WRITE, 0, R32G32B32A32_U, 0, 0, 0, TU_SHADER_RES, TEXTURE2D)
	{};
	Texture& Texture::NullTexture()
	{
		static NTexture Instance;
		return Instance;
	}

}
