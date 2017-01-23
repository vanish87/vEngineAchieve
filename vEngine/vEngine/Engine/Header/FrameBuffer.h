#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#pragma once

#include <vector>

#include "EnginePreDec.h"
#include "RenderView.h"
#include "Viewport.h"

namespace vEngine
{
	class FrameBuffer
	{
	public:
		FrameBuffer(uint32_t width, uint32_t height);
		virtual ~FrameBuffer(void);

		Viewport& GetViewport();

		virtual void OnBind() = 0;
		virtual void AddRenderView(RenderView* render_view);
		virtual RenderView* GetRenderView(size_t index){return render_views_[index];};
		Texture* GetDepthTexture(){return depth_texture_;};

	private:
		FrameBuffer(void);

	protected:
		uint32_t width_;
		uint32_t height_;

		Viewport viewport_;

		std::vector<RenderView*> render_views_;
		Texture* depth_texture_;
	};
}

#endif

