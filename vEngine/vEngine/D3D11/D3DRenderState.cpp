#include "D3DRenderState.h"
#include "Engine\Header\Context.h"
#include "D3DRenderEngine.h"

namespace vEngine
{
	D3DRenderState::D3DRenderState(void) 
		:rasterizer_state_(nullptr), 
		 depth_stencil_state_(nullptr),
		 blend_state_(nullptr)
	{
	}

	D3DRenderState::D3DRenderState( D3D11_RASTERIZER_DESC desc )
	{
		D3DRenderState();
		state_type_ = RST_RASTERIZER;		
		D3DRenderEngine* d3d_re =static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		HRESULT res = d3d_re->D3DDevice()->CreateRasterizerState(&desc, &rasterizer_state_);
		if(FAILED(res))PRINT("CreateRasterizerState Failed");
	}

	D3DRenderState::D3DRenderState( D3D11_DEPTH_STENCIL_DESC desc )
	{
		D3DRenderState();
		state_type_ = RST_DEPTH;	
		D3DRenderEngine* d3d_re =static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		d3d_re->D3DDevice()->CreateDepthStencilState(&desc, &depth_stencil_state_);
	}

	D3DRenderState::D3DRenderState( D3D11_BLEND_DESC desc )
	{
		D3DRenderState();
		state_type_ = RST_BLEND;	
	}


	D3DRenderState::~D3DRenderState(void)
	{
	}

	ID3D11RasterizerState* D3DRenderState::GetRasterizerState()
	{
		assert(state_type_ == RST_RASTERIZER);
		return rasterizer_state_;
	}

	ID3D11DepthStencilState* D3DRenderState::GetDepthStencilState()
	{
		assert(state_type_ == RST_DEPTH);
		return depth_stencil_state_;
	}

	ID3D11BlendState* D3DRenderState::GetBlendState()
	{
		assert(state_type_ == RST_BLEND);
		return blend_state_;
	}

}
