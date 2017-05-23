#ifndef D3DFRAMEBUFFER_H_
#define D3DFRAMEBUFFER_H_

#pragma once
#include "Engine\Header\FrameBuffer.h"
#include "D3DPreDec.h"
#include "D3DRenderView.h"
#include "D3DTexture.h"
#include "D3DRenderEngine.h"
#include "Engine\Header\Context.h"

namespace vEngine
{

	class D3DFrameBuffer : public FrameBuffer
	{
	public:
		D3DFrameBuffer(uint32_t width, uint32_t height);
		~D3DFrameBuffer(void);

		//D3DRenderTargetView* const D3DRTView(size_t index = 0) const {return static_cast<D3DRenderTargetView*>(render_views_[index]);};
		//size_t D3DRTViewSize(){return render_views_.size();};
		//D3DDepthStencilRenderView* const D3DDSView() const {return static_cast<D3DDepthStencilRenderView*>(depth_stencil_view_);};
		//D3DTexture2D* const &D3DDSBuffer() const {return depth_texture_;};

		virtual void OnBind() override;
		virtual void Clear(float4 color, float depth, uint32_t mask) override;

	private:
		D3DFrameBuffer(void);
	};
}


#endif
