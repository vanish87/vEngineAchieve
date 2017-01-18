#include "D3DFrameBuffer.h"

namespace vEngine
{
	D3DFrameBuffer::D3DFrameBuffer(void)
		: FrameBuffer(0, 0,1280, 800),
		  depth_stencil_view_(new D3DDepthStencilRenderView)
	{
		depth_texture_ = new D3DTexture2D();
		D3D11_TEXTURE2D_DESC depth_stencil_desc;

		depth_stencil_desc.Width     = render_views_[0]->Width();
		depth_stencil_desc.Height    = render_views_[0]->Height();
		depth_stencil_desc.MipLevels = 1;
		depth_stencil_desc.ArraySize = 1;
		depth_stencil_desc.Format    = DXGI_FORMAT_R24G8_TYPELESS;
		depth_stencil_desc.SampleDesc.Count   = 1;
		depth_stencil_desc.SampleDesc.Quality = 0;
		depth_stencil_desc.Usage          = D3D11_USAGE_DEFAULT;
		depth_stencil_desc.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depth_stencil_desc.CPUAccessFlags = 0; 
		depth_stencil_desc.MiscFlags      = 0;
		ID3D11Texture2D* depth_stencil_buffer;
		ID3D11DepthStencilView*	depth_stencil_view;
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		HRESULT result = d3d_re->D3DDevice()->CreateTexture2D(&depth_stencil_desc, 0, &depth_stencil_buffer);
		if(FAILED(result))
			PRINT("depth_stencil create Failed!");
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));
		dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0;
		result = d3d_re->D3DDevice()->CreateDepthStencilView(depth_stencil_buffer, &dsvd, &depth_stencil_view);
		if(FAILED(result))
			PRINT("depth_stencil_view create Failed!");

		this->depth_stencil_view_->SetD3DDSV(depth_stencil_view);
		static_cast<D3DTexture2D*>(this->depth_texture_)->SetD3DTexture(depth_stencil_buffer);
		this->depth_texture_->SetUsage(TU_DEPTH_SR);
	}

	D3DFrameBuffer::D3DFrameBuffer( Configure::RenderSetting& render_setting )
		: FrameBuffer(render_setting.left, render_setting.top, render_setting.width, render_setting.height),
		  depth_stencil_view_(new D3DDepthStencilRenderView)		
	{
		depth_texture_ = new D3DTexture2D();
		D3D11_TEXTURE2D_DESC depth_stencil_desc;

		depth_stencil_desc.Width     = render_setting.width;
		depth_stencil_desc.Height    = render_setting.height;
		depth_stencil_desc.MipLevels = 1;
		depth_stencil_desc.ArraySize = 1;
		depth_stencil_desc.Format    = DXGI_FORMAT_R24G8_TYPELESS;
		depth_stencil_desc.SampleDesc.Count   = 1;
		depth_stencil_desc.SampleDesc.Quality = 0;
		depth_stencil_desc.Usage          = D3D11_USAGE_DEFAULT;
		depth_stencil_desc.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depth_stencil_desc.CPUAccessFlags = 0; 
		depth_stencil_desc.MiscFlags      = 0;
		ID3D11Texture2D* depth_stencil_buffer;
		ID3D11DepthStencilView*	depth_stencil_view;
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		HRESULT result = d3d_re->D3DDevice()->CreateTexture2D(&depth_stencil_desc, 0, &depth_stencil_buffer);
		if(FAILED(result))
			PRINT("depth_stencil create Failed!");
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));
		dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0;
		result = d3d_re->D3DDevice()->CreateDepthStencilView(depth_stencil_buffer, &dsvd, &depth_stencil_view);
		if(FAILED(result))
			PRINT("depth_stencil_view create Failed!");

		this->depth_stencil_view_->SetD3DDSV(depth_stencil_view);
		static_cast<D3DTexture2D*>(this->depth_texture_)->SetD3DTexture(depth_stencil_buffer);
		this->depth_texture_->SetUsage(TU_DEPTH_SR);
		//TODO : move it to Base Case
		viewport_ = new Viewport(0 , 0,render_setting.width,render_setting.height);
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
		ID3D11DepthStencilView* dsv = this->depth_stencil_view_->D3DDSV();
		if(dsv == nullptr)
		{
			D3D11_TEXTURE2D_DESC depth_stencil_desc;

			depth_stencil_desc.Width     = render_views_[0]->Width();
			depth_stencil_desc.Height    = render_views_[0]->Height();
			depth_stencil_desc.MipLevels = 1;
			depth_stencil_desc.ArraySize = 1;
			depth_stencil_desc.Format    = DXGI_FORMAT_R24G8_TYPELESS;
			depth_stencil_desc.SampleDesc.Count   = 1;
			depth_stencil_desc.SampleDesc.Quality = 0;
			depth_stencil_desc.Usage          = D3D11_USAGE_DEFAULT;
			depth_stencil_desc.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			depth_stencil_desc.CPUAccessFlags = 0; 
			depth_stencil_desc.MiscFlags      = 0;
			ID3D11Texture2D* depth_stencil_buffer;
			ID3D11DepthStencilView*	depth_stencil_view;
			D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
			HRESULT result = d3d_re->D3DDevice()->CreateTexture2D(&depth_stencil_desc, 0, &depth_stencil_buffer);
			if(FAILED(result))
				PRINT("depth_stencil create Failed!");
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
			ZeroMemory(&dsvd, sizeof(dsvd));
			dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvd.Texture2D.MipSlice = 0;
			result = d3d_re->D3DDevice()->CreateDepthStencilView(depth_stencil_buffer, &dsvd, &depth_stencil_view);
			if(FAILED(result))
				PRINT("depth_stencil_view create Failed!");

			this->depth_stencil_view_->SetD3DDSV(depth_stencil_view);
			static_cast<D3DTexture2D*>(this->depth_texture_)->SetD3DTexture(depth_stencil_buffer);
			this->depth_texture_->SetUsage(TU_DEPTH_SR);
		}
		dsv = this->depth_stencil_view_->D3DDSV();
		//only draw depth
		if(rtvs.size() == 0)
		{
			render_engine->D3DDeviceImmContext()->OMSetRenderTargets(0, NULL, dsv);
		}
		else
			render_engine->D3DDeviceImmContext()->OMSetRenderTargets(rtvs.size(), &rtvs.front() , dsv);

		D3D11_VIEWPORT d3d_viewport;
		d3d_viewport.TopLeftX = viewport_->Top();
		d3d_viewport.TopLeftY = viewport_->Left();
		d3d_viewport.Width    = static_cast<float>(viewport_->Width());
		d3d_viewport.Height   = static_cast<float>(viewport_->Height());
		d3d_viewport.MinDepth = 0.0f;//get info from frustum
		d3d_viewport.MaxDepth = 1.0f;

		render_engine->D3DDeviceImmContext()->RSSetViewports(1, &d3d_viewport);
	}

}
