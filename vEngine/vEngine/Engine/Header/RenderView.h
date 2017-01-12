#ifndef RENDERVIEW_H_
#define RENDERVIEW_H_

#pragma once

#include "EnginePreDec.h"
#include "TypeAndFormat.h"

namespace MocapGE
{
	class RenderView
	{
	public:
		RenderView(void);
		~RenderView(void);

		size_t Width(){return width_;};
		size_t Height(){return height_;};

		Texture* GetTexture(){return texture_;};

	protected:
		Format format_;
		size_t height_;
		size_t width_;

		size_t dimension_; 

		Texture* texture_;
	};

}

#endif

