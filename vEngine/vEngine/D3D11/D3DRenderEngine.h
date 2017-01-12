#ifndef D3DRENDERENGINE_H_
#define D3DRENDERENGINE_H_

#pragma once

#include "Engine\Header\EnginePreDec.h"
#include "Engine\Header\Context.h"
#include "Engine\Header\App.h"
#include "Engine\Header\Window.h"
#include "Engine\Header\RenderEngine.h"
#include "D3DPreDec.h"
#include "Engine\Header\TypeAndFormat.h"
#include "D3DRenderLayout.h"
#include "D3DFrameBuffer.h"
#include "D3DShaderobject.h"
#include "D3DRenderBuffer.h"
#include "D3DRenderState.h"

#pragma comment(lib, "d3d11.lib") //may fix in the future
#pragma comment( lib, "dxguid.lib") 

namespace MocapGE
{
	class D3DRenderEngine :	public RenderEngine
	{
	public:
		D3DRenderEngine(void);
		~D3DRenderEngine(void);

		virtual void InitRenderWindow( std::string const & name, Configure::RenderSetting const & render_setting );

		virtual void Render(RenderLayout* render_layout, ShaderObject* shader_object, int pass_index); 

		virtual void SwapBuffers();

		virtual void OnResize();

		ID3D11Device* const D3DDevice() const{return d3d_device_;};
		ID3D11DeviceContext* const & D3DDeviceImmContext() const{return d3d_imm_context_;};

		DXGI_FORMAT MapFormat(Format format);

		virtual void BindFrameBuffer( FrameBuffer* const & fb );

		virtual void RenderFrameBegin();

		virtual void RenderFrameEnd();

		virtual void SetDeferredRenderingState();

		virtual void SetNormalState();

		virtual void TrunoffCull();

	private:
		ID3D11Device*		d3d_device_;
		ID3D11DeviceContext* d3d_imm_context_;
		D3D_FEATURE_LEVEL d3d_feature_level_;
		IDXGISwapChain* d3d_swap_chain;	

		D3DRenderState* depth_on_;
		D3DRenderState* depth_off_;
		D3DRenderState* wire_frame_;
		D3DRenderState* cull_back_;
		D3DRenderState* cull_depth_;

	};
}


#endif

