#include "D3DRenderView.h"
#include "D3DRenderEngine.h"

namespace vEngine
{

	/*D3DRenderTargetView::D3DRenderTargetView(void)
		:render_target_view_(nullptr)
	{
	}

	D3DRenderTargetView::D3DRenderTargetView( Texture& texture, int array_size, int mip_level )
	{
		texture_ = &texture;
		D3DTexture2D* d3d_t2d = static_cast<D3DTexture2D*>(&texture);
		render_target_view_ = d3d_t2d->GetRenderTargetView( array_size, mip_level ,texture_->GetType());
		width_ = d3d_t2d->GetWidth();
		height_ = d3d_t2d->GetHeight();

		format_ = d3d_t2d->GetFormat();
		dimension_ = TEXTURE2D;
	}


	D3DRenderTargetView::~D3DRenderTargetView(void)
	{
	}

	void D3DRenderTargetView::SetD3DRTV( ID3D11RenderTargetView* render_target_view )
	{
		this->render_target_view_ = render_target_view;
	}

	D3DDepthStencilRenderView::D3DDepthStencilRenderView( void )
		:depth_stencil_view_(nullptr)
	{

	}
	
	D3DDepthStencilRenderView::D3DDepthStencilRenderView(Texture& texture, int array_size, int mip_level, DepthStencilUsage usage)
	{
		CHECK_ASSERT(texture.GetType() == TEXTURE2D);
		D3DTexture2D* d3d_texture = static_cast<D3DTexture2D*>(&texture);
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));
		//TODO check format compatibility
		dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//d3d_re->MapFormat(d3d_texture->GetFormat());
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = mip_level;
		HRESULT result = d3d_re->D3DDevice()->CreateDepthStencilView(d3d_texture->D3DTexture(), &dsvd, &depth_stencil_view_);
		if (FAILED(result))
			PRINT("depth_stencil_view create Failed!");
	}

	D3DDepthStencilRenderView::~D3DDepthStencilRenderView(void)
	{

	}

	void D3DDepthStencilRenderView::SetD3DDSV(ID3D11DepthStencilView* depth_stencil_view)
	{
		this->depth_stencil_view_=depth_stencil_view;
	}
*/

}
