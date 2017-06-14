#ifndef D3DRENDERSTATE_H_
#define D3DRENDERSTATE_H_

#pragma once

#include "Engine/Header/RenderState.h"
#include "D3DPreDec.h"


namespace vEngine
{
	//TODO : use different D3D class to implement different RenderState
	class D3DRenderState : public RenderState
	{
	public:
		D3DRenderState(void);
		D3DRenderState(D3D11_RASTERIZER_DESC desc);
		D3DRenderState(D3D11_DEPTH_STENCIL_DESC desc);
		D3DRenderState(D3D11_BLEND_DESC desc);
		~D3DRenderState(void);

		ID3D11RasterizerState* GetRasterizerState();
		ID3D11DepthStencilState* GetDepthStencilState();
		ID3D11BlendState* GetBlendState();

	private:
		ID3D11RasterizerState* rasterizer_state_;
		ID3D11DepthStencilState* depth_stencil_state_;
		ID3D11BlendState* blend_state_;
	};

}
#endif
