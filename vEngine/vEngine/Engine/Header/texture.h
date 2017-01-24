#ifndef TEXTURE_H_
#define TEXTURE_H_

#pragma once

#include "EnginePreDec.h"
#include "TypeAndFormat.h"

namespace vEngine
{
	class Texture
	{
	public:
		Texture(void);
		Texture(AccessType access_type, uint32_t array_size, 
				Format format, uint32_t mip_level, uint32_t sample_count, uint32_t sample_quality, TextureUsage usage, TextureType type);
		virtual ~Texture(void);

		void SetUsage(TextureUsage usage);
		TextureType GetType();

		static Texture& NullTexture();
	protected:
		AccessType access_type_;
		uint32_t array_size_;
		Format format_;
		uint32_t mip_level_;
		uint32_t sample_count_;
		uint32_t sample_quality_;
		TextureUsage usage_;
		TextureType type_;
	};

	class NTexture :public Texture
	{
	public:
		NTexture();
		~NTexture();
	};

}

#endif

