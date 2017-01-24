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
		this->depth_stencil_view_ = Context::Instance().GetRenderFactory().MakeRenderView(this->depth_texture_, 0, 0, DSU_BOTH);
	}


	D3DFrameBuffer::~D3DFrameBuffer(void)
	{
	}

	void D3DFrameBuffer::OnBind()
	{
		D3DRenderEngine* render_engine = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		std::vector<ID3D11RenderTargetView*> rtvs;
		for(size_t i = 0; i< render_views_.size(); i++)
			rtvs.push_back(static_cast<D3DRenderTargetView*>(this->render_views_[i])->D3DRTV());
		D3DDepthStencilRenderView* depth_view = static_cast<D3DDepthStencilRenderView*>(depth_stencil_view_);
		ID3D11DepthStencilView* dsv = depth_view->D3DDSV();
		assert(dsv != nullptr);
		
		dsv = depth_view->D3DDSV();
		//only draw depth
		if(rtvs.size() == 0)
		{
			render_engine->D3DDeviceImmContext()->OMSetRenderTargets(0, NULL, dsv);
		}
		else
		{
			render_engine->D3DDeviceImmContext()->OMSetRenderTargets((uint32_t)rtvs.size(), &rtvs.front(), dsv);
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
			for (size_t i = 0; i < this->D3DRTViewSize(); i++)
				render_engine->D3DDeviceImmContext()->ClearRenderTargetView(D3DRTView(i)->D3DRTV(), c);
		}

		render_engine->D3DDeviceImmContext()->ClearDepthStencilView(D3DDSView()->D3DDSV(), mask, depth, 0);
	}

}
