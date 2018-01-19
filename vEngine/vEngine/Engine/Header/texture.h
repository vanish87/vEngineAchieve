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
		TextureUsage GetUsage();
		TextureType GetType();

		void* Map(AccessType access_type)
		{
			this->DoMap(access_type);
			CHECK_ASSERT(data_);
			return data_;
		};
		virtual void UnMap() = 0;

		static Texture& NullTexture();
	protected:
		//assign data_ with mapped resource
		virtual void DoMap(AccessType access_type) = 0;

	protected:

		AccessType access_type_;
		uint32_t array_size_;
		Format format_;
		uint32_t mip_level_;
		uint32_t sample_count_;
		uint32_t sample_quality_;
		TextureUsage usage_;
		TextureType type_;

		//optional cpu data
		void* data_;
	};

	class NTexture :public Texture
	{
	public:
		NTexture();

		~NTexture()
		{

		};


	protected:

		virtual void DoMap(AccessType access_type) { CHECK_ASSERT(false); };
		virtual void UnMap() { CHECK_ASSERT(false); };

	};

}

#endif

