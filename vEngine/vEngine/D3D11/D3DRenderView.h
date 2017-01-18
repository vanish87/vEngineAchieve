#ifndef D3DRENDERVIEW_H_
#define D3DRENDERVIEW_H_

#pragma once

#include "Engine\Header\RenderView.h"

#include "D3DPreDec.h"
#include "D3DTexture.h"

namespace vEngine
{
	class D3DRenderTargetView :		public RenderView
	{
	public:
		D3DRenderTargetView(void);
		~D3DRenderTargetView(void);

		D3DRenderTargetView(Texture& texture, int array_size, int mip_level );
		D3DRenderTargetView(D3DRenderTargetView* render_target_view);

		ID3D11RenderTargetView* const &D3DRTV() const{return render_target_view_;};
		void SetD3DRTV( ID3D11RenderTargetView* render_target_view );

	private:
		ID3D11RenderTargetView* render_target_view_;
	};

	class D3DDepthStencilRenderView : public RenderView
	{
	public:
		D3DDepthStencilRenderView(void);
		~D3DDepthStencilRenderView(void);

		ID3D11DepthStencilView* const &D3DDSV(){return depth_stencil_view_;};
		void SetD3DDSV( ID3D11DepthStencilView* depth_stencil_view );
	private:
		ID3D11DepthStencilView* depth_stencil_view_;
	};




}

#endif

