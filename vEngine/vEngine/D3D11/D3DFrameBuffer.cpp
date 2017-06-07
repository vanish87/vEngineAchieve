#include "D3DFrameBuffer.h"

namespace vEngine
{
	D3DFrameBuffer::D3DFrameBuffer(void)
		: D3DFrameBuffer(1280, 800)
	{
		//invalid one
		//private function
	}

	D3DFrameBuffer::D3DFrameBuffer(uint32_t width, uint32_t height)
		: FrameBuffer(width, height)
	{
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());

		this->depth_texture_ = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, width, height, 1, 1, R24G8_TYPELESS, 1, 0, AT_GPU_READ_WRITE, TU_DEPTH_SR);
		//this->depth_stencil_view_ = Context::Instance().GetRenderFactory().MakeRenderView(this->depth_texture_, 0, 0, DSU_BOTH);
	}


	D3DFrameBuffer::~D3DFrameBuffer(void)
	{
	}

	void D3DFrameBuffer::OnBind()
	{
		D3DRenderEngine* render_engine = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		std::vector<ID3D11RenderTargetView*> rtvs;
		for (size_t i = 0; i < render_texture_.size(); i++)
		{
			D3DTexture2D* tex = static_cast<D3DTexture2D*>(this->render_texture_[i]);
			if (this->GetViewport().Height() != tex->GetHeight()
				|| this->GetViewport().Width() != tex->GetWidth()
				)
			{
				delete this->depth_texture_;
				viewport_.SetWidth(tex->GetHeight());
				viewport_.SetHeight(tex->GetWidth());
				this->depth_texture_ = Context::Instance().GetRenderFactory().MakeTexture2D(nullptr, this->GetViewport().Width(), this->GetViewport().Height(), 1, 1, R24G8_TYPELESS, 1, 0, AT_GPU_READ_WRITE, TU_DEPTH_SR);
			}

			rtvs.push_back(tex->GetRenderTargetView(1, 0, TEXTURE2D));
		}
		D3DTexture2D* depth_tex = static_cast<D3DTexture2D*>(this->depth_texture_);
		
		//only draw depth
		if(rtvs.size() == 0)
		{
			render_engine->D3DDeviceImmContext()->OMSetRenderTargets(0, NULL, depth_tex->GetDepthStencilView(1, 0, TEXTURE2D));
		}
		else
		{
			render_engine->D3DDeviceImmContext()->OMSetRenderTargets((uint32_t)rtvs.size(), &rtvs.front(), depth_tex->GetDepthStencilView(1, 0, TEXTURE2D));
		}

		D3D11_VIEWPORT d3d_viewport;
		d3d_viewport.TopLeftX = static_cast<float>(viewport_.Top());
		d3d_viewport.TopLeftY = static_cast<float>(viewport_.Left());
		d3d_viewport.Width    = static_cast<float>(viewport_.Width()); 
		d3d_viewport.Height   = static_cast<float>(viewport_.Height());
		d3d_viewport.MinDepth = 0.0f;//get info from frustum
		d3d_viewport.MaxDepth = 1.0f;

		render_engine->D3DDeviceImmContext()->RSSetViewports(1, &d3d_viewport);
	}

	void D3DFrameBuffer::Clear(float4 color, float depth, uint32_t mask)
	{
		float c[4] = {color.x(), color.y() ,color.z() ,color.w() };
		D3DRenderEngine* render_engine = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		if ((mask & CBM_COLOR) != 0)
		{
			for (size_t i = 0; i < this->render_texture_.size(); i++)
				render_engine->D3DDeviceImmContext()->ClearRenderTargetView(static_cast<D3DTexture2D*>(this->render_texture_[i])->GetRenderTargetView(1, 0, TEXTURE2D), c);
		}
		D3DTexture2D* depth_tex = static_cast<D3DTexture2D*>(this->depth_texture_);

		render_engine->D3DDeviceImmContext()->ClearDepthStencilView(depth_tex->GetDepthStencilView(1, 0, TEXTURE2D), mask, depth, 0);
	}

}
