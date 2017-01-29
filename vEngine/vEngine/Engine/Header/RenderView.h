#ifndef RENDERVIEW_H_
#define RENDERVIEW_H_

#pragma once

#include "EnginePreDec.h"
#include "TypeAndFormat.h"

namespace vEngine
{
	class RenderView
	{
	public:
		RenderView(void);
		virtual ~RenderView(void);

		uint32_t Width(){return width_;};
		uint32_t Height(){return height_;};

		Texture* GetTexture(){return texture_;};

	protected:
		Format format_;
		uint32_t height_;
		uint32_t width_;

		size_t dimension_; 

		Texture* texture_;
	};

}

#endif

