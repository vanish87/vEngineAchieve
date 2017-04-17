#include "D3DRenderEngine.h"


namespace vEngine
{
	D3DRenderEngine::D3DRenderEngine(void)
	{		
		cur_frame_buffer_ = nullptr;
	}


	D3DRenderEngine::~D3DRenderEngine(void)
	{

	}

	void D3DRenderEngine::InitRenderWindow( std::string const & name, Configure::RenderSetting const & render_setting )
	{
		RenderEngine::InitRenderWindow(name, render_setting);

		HRESULT result;
		UINT create_device_flags = 0;
		#if defined(_DEBUG)  
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		result = D3D11CreateDevice(
				nullptr,                 // default adapter
				md3dDriverType,
				nullptr,                 // no software device
				create_device_flags, 
				nullptr, 0,				//  D11 feature level array
				D3D11_SDK_VERSION,
				&d3d_device_,
				&d3d_feature_level_,
				&d3d_imm_context_);

		if( FAILED(result) )
		{
			PRINT("D3D11CreateDevice Failed.");
		}

		if( d3d_feature_level_ < D3D_FEATURE_LEVEL_11_0)
		{
			PRINT_AND_RETURN("Direct3D Feature Level 11 unsupported.",);
		}
		

		DXGI_SWAP_CHAIN_DESC swap_chain_desc;
		ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
		swap_chain_desc.BufferDesc.Width  = this->render_setting_.width;
		swap_chain_desc.BufferDesc.Height = this->render_setting_.height;
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		UINT msaa_quality = 0;
		result = d3d_device_->CheckMultisampleQualityLevels(swap_chain_desc.BufferDesc.Format, 4, &msaa_quality);
		if (FAILED(result) || msaa_quality == 0)
		{
			PRINT("msaa_quality Failed.");
		}

		// Use 4X MSAA? 
		if( this->render_setting_.msaa4x )
		{
			swap_chain_desc.SampleDesc.Count   = 4;
			swap_chain_desc.SampleDesc.Quality = msaa_quality-1;
		}
		// No MSAA
		else
		{
			swap_chain_desc.SampleDesc.Count   = 1;
			swap_chain_desc.SampleDesc.Quality = 0;
		}
		swap_chain_desc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.BufferCount  = 1;
		swap_chain_desc.OutputWindow = Context::Instance().GetAppInstance().GetWindow().GetWnd();
		swap_chain_desc.Windowed     = !this->render_setting_.full_screen;
		swap_chain_desc.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
		swap_chain_desc.Flags        = 0;

		IDXGIDevice* dxgi_device = 0;
		result = d3d_device_->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device);
		if( FAILED(result) )
		{
			PRINT("dxgiDevice Failed.");
		}

		IDXGIAdapter* dxgi_adapter = 0;
		result = dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgi_adapter);
		if( FAILED(result) )
		{
			PRINT("dxgiAdapter Failed.");
		}

		IDXGIFactory1* dxgiFactory = 0;
		result = dxgi_adapter->GetParent(__uuidof(IDXGIFactory1), (void**)&dxgiFactory);
		if( FAILED(result) )
		{
			PRINT("dxgiFactory Failed.");
		}
		//optional IDXGIFactory2 not used until D3D11.1 or later feature
		IDXGIFactory2* dxgiFactory2 = nullptr;
		result = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));

		result = dxgiFactory->CreateSwapChain(d3d_device_, &swap_chain_desc, &d3d_swap_chain_);

		if( FAILED(result) )
		{
			PRINT("d3d_swap_chain Failed.");
		}

		dxgi_device->Release();
		dxgi_adapter->Release();
		dxgiFactory->Release();
		if (dxgiFactory2 != nullptr) dxgiFactory2->Release();

		this->OnResize();

	}


	void D3DRenderEngine::Render(RenderLayout* render_layout, ShaderObject* shader_object, int pass_index)
	{
		HRESULT result;
		//Get view and Projection Matrix
		D3DFrameBuffer* d3d_frame_buffer;
		d3d_frame_buffer= static_cast<D3DFrameBuffer*>(cur_frame_buffer_);
		float4x4 view_mat = d3d_frame_buffer->GetViewport().GetCamera().GetViewMatirx();
		float4x4 proj_mat = d3d_frame_buffer->GetViewport().GetCamera().GetProjMatrix();
		float3 camera_pos = d3d_frame_buffer->GetViewport().GetCamera().GetPos();
		float3 camera_at =  d3d_frame_buffer->GetViewport().GetCamera().GetLookAt();

		//Make sure every Shader has a constant named view_proj_matrix
		shader_object->SetMatrixVariable("g_view_proj_matrix", view_mat*proj_mat);
		shader_object->SetMatrixVariable("g_view_matrix",view_mat);
		shader_object->SetMatrixVariable("g_inv_proj_matrix", Math::Inverse(proj_mat));
		shader_object->SetMatrixVariable("g_inv_view_matrix", Math::Inverse(view_mat));
		shader_object->SetVectorVariable("g_eye_pos", camera_pos);
		

		D3DShaderobject* d3d_shader_object = static_cast<D3DShaderobject*>(shader_object);
		
		//IASetInputLayout
		D3DRenderLayout* d3d_rl = static_cast<D3DRenderLayout*>(render_layout);
		std::vector<VertexUsage> vertex_layout = d3d_rl->GetInputLayout();
		D3D11_INPUT_ELEMENT_DESC *input_layout_desc = new D3D11_INPUT_ELEMENT_DESC[vertex_layout.size()]; 
		for (size_t i = 0; i < vertex_layout.size(); i++)
		{
			input_layout_desc[i].SemanticIndex = 0;
			input_layout_desc[i].InputSlot = 0;
			input_layout_desc[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			input_layout_desc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			input_layout_desc[i].InstanceDataStepRate = 0;
			switch (vertex_layout[i])
			{
			case VU_POSITION:
				input_layout_desc[i].SemanticName = "POSITION";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case VU_NORMAL:
				input_layout_desc[i].SemanticName = "NORMAL";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case VU_TEXCOORD:
				input_layout_desc[i].SemanticName = "TEXCOORD";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32_FLOAT;
				break;
			case VU_TANGENT:
				input_layout_desc[i].SemanticName = "TANGENT";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case VU_BINORMAL:
				input_layout_desc[i].SemanticName = "BINORMAL";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case VU_COLOR:
				input_layout_desc[i].SemanticName = "COLOR";
				input_layout_desc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			default:
				break;
			}


		}
		// Create the input layout
		D3DX11_PASS_DESC pass_desc;
		result = d3d_shader_object->GetTechnique()->GetPassByIndex(pass_index)->GetDesc( &pass_desc );
		if(FAILED(result))PRINT("Cannot Get Pass Desc");
		ID3D11InputLayout* input_layout;
		result = d3d_device_->CreateInputLayout(input_layout_desc, (uint32_t)vertex_layout.size(), pass_desc.pIAInputSignature, 
			pass_desc.IAInputSignatureSize, &input_layout);
		if(FAILED(result))
		{
			result = d3d_device_->GetDeviceRemovedReason();
			PRINT("Cannot Create Input Layout");
		}
		d3d_imm_context_->IASetInputLayout(input_layout);
		input_layout->Release();
		delete[] input_layout_desc;

		//IASetPrimitiveTopology
		PrimitiveType pri_type = d3d_rl->GetPrimitive();
		switch (pri_type)
		{
		case PT_POINTLIST:
			d3d_imm_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			break;
		case PT_TRIANGLELIST:
			d3d_imm_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;
		case PT_LINELIST:
			d3d_imm_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			break;
		default:
			break;
		}
		{							
			//IASetVertexBuffers
			D3DRenderBuffer* d3d_vertex_buffer = static_cast<D3DRenderBuffer*>(d3d_rl->GetBuffer(VBU_VERTEX));
			uint32_t stride = d3d_rl->GetVertexSize();
			uint32_t offset = 0;
			d3d_imm_context_->IASetVertexBuffers(0, 1, &(d3d_vertex_buffer->D3DBuffer()), &stride, &offset);

			//IASetIndexBuffer	
			D3DRenderBuffer* d3d_index_buffer = static_cast<D3DRenderBuffer*>(d3d_rl->GetBuffer(VBU_INDEX));
			d3d_imm_context_->IASetIndexBuffer(d3d_index_buffer->D3DBuffer(), DXGI_FORMAT_R32_UINT, 0);
			
			//DrawIndexed
			shader_object->Apply(pass_index);
			uint32_t index_count = d3d_rl->GetIndexCount();	
			d3d_imm_context_->DrawIndexed(index_count, 0, 0);
		}

	}

	void D3DRenderEngine::SwapBuffers()
	{
		HRESULT result = d3d_swap_chain_->Present(0, 0);
		if(FAILED(result))
		{
			result = d3d_device_->GetDeviceRemovedReason();
			PRINT("d3d_swap_chain->Present Failed");
		}
	}

	void D3DRenderEngine::OnResize()
	{
		HRESULT result;
		D3DFrameBuffer* d3d_frame_buffer;
		if(cur_frame_buffer_ == nullptr)
		{
			cur_frame_buffer_ = Context::Instance().GetRenderFactory().MakeFrameBuffer(render_setting_.width, render_setting_.height);
		    d3d_frame_buffer= static_cast<D3DFrameBuffer*>(cur_frame_buffer_);
		}
		else
		{
			//check it
			d3d_frame_buffer= static_cast<D3DFrameBuffer*>(cur_frame_buffer_);
			d3d_frame_buffer->D3DRTView()->D3DRTV()->Release();
			d3d_frame_buffer->D3DDSView()->D3DDSV()->Release();
			//
			PRINT_AND_BREAK("This should be check and make a new MakeFrameBuffer here");
			//then the rest of this function should only handle render targets
		}

		//TODO : Use new size of window to resize FrameBuffer
		result = d3d_swap_chain_->ResizeBuffers(1, this->render_setting_.width, this->render_setting_.height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		if(FAILED(result))
			PRINT("ResizeBuffer Failed!");

		ID3D11Texture2D* back_buffer = nullptr;
		ID3D11RenderTargetView* render_target_view = nullptr;
		result = d3d_swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
		if(FAILED(result))
			PRINT("Get back Buffer Failed!");
		
		Texture* d3d_tex = Context::Instance().GetRenderFactory().MakeTexture2D(back_buffer);
		RenderView* render_view = Context::Instance().GetRenderFactory().MakeRenderView(d3d_tex, 1, 0);
		d3d_frame_buffer->AddRenderView(render_view);
		//it could be used later
		//back_buffer->Release();

		this->BindFrameBuffer(d3d_frame_buffer);


		//TODO have some good way to maintain varies of RenderState

		// Set up the description of the stencil state.
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = false;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depth_on_ = new D3DRenderState(depthStencilDesc);

		D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
		depthDisabledStencilDesc.DepthEnable = false;
		depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthDisabledStencilDesc.StencilEnable = false;
		depthDisabledStencilDesc.StencilReadMask = 0xFF;
		depthDisabledStencilDesc.StencilWriteMask = 0xFF;
		depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depth_off_ = new D3DRenderState(depthDisabledStencilDesc);

		D3D11_RASTERIZER_DESC wireframeDesc;
		ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
		wireframeDesc.FillMode = D3D11_FILL_SOLID;
		wireframeDesc.CullMode = D3D11_CULL_BACK;
		wireframeDesc.FrontCounterClockwise = false;
		wireframeDesc.DepthClipEnable = true;
		//wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;

		wire_frame_ = new D3DRenderState(wireframeDesc);


		depthDisabledStencilDesc.DepthEnable = false;
		depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthDisabledStencilDesc.StencilEnable = false;
		depthDisabledStencilDesc.StencilReadMask = 0xFF;
		depthDisabledStencilDesc.StencilWriteMask = 0xFF;
		depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		cull_depth_ = new D3DRenderState(depthDisabledStencilDesc);

		D3D11_RASTERIZER_DESC cull_Desc;
		ZeroMemory(&cull_Desc, sizeof(D3D11_RASTERIZER_DESC));
		cull_Desc.FillMode = D3D11_FILL_SOLID;
		cull_Desc.CullMode = D3D11_CULL_NONE;
		cull_Desc.FrontCounterClockwise = false;
		cull_Desc.DepthClipEnable = true;
		cull_Desc.DepthBias = 0;
		cull_Desc.DepthBiasClamp = 0.0f;
		cull_Desc.MultisampleEnable = false;
		cull_Desc.ScissorEnable = false;
		cull_Desc.SlopeScaledDepthBias = 0.0f;
		cull_back_ = new D3DRenderState(cull_Desc);

	}

	DXGI_FORMAT D3DRenderEngine::MapFormat( Format format)
	{
		switch (format)
		{
		case A8_U:
			return DXGI_FORMAT_A8_UNORM;
		case R8_U:
			return DXGI_FORMAT_R8_UNORM;
		case R8G8B8A8_U:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case R32G32B32A32_F:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case D24_U_S8_U:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case R24G8_TYPELESS:
			return DXGI_FORMAT_R24G8_TYPELESS;
		default:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		}
	}

	Format D3DRenderEngine::ReverseMapFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_A8_UNORM:
			return A8_U;
		case DXGI_FORMAT_R8_UNORM:
			return R8_U;
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return R8G8B8A8_U;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return R32G32B32A32_F;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			return D24_U_S8_U;
		case DXGI_FORMAT_R24G8_TYPELESS:
			return R24G8_TYPELESS;
		default:
			PRINT_AND_BREAK("Should handle this");
			return R32G32B32A32_U;
		}
	}

	void D3DRenderEngine::BindFrameBuffer(FrameBuffer* const & fb)
	{
		cur_frame_buffer_ = fb;
		fb->OnBind();
	}

	void D3DRenderEngine::RenderFrameBegin()
	{
		//Clear Frame Buffer
		cur_frame_buffer_->Clear(float4(0, 0, 0, 1), 1, FrameBuffer::CBM_DEPTH | FrameBuffer::CBM_STENCIL | FrameBuffer::CBM_COLOR);
	}

	void D3DRenderEngine::RenderFrameEnd()
	{
		// Cleanup (aka make the runtime happy)
		d3d_imm_context_->VSSetShader(0, 0, 0);
		d3d_imm_context_->GSSetShader(0, 0, 0);
		d3d_imm_context_->PSSetShader(0, 0, 0);
		d3d_imm_context_->OMSetRenderTargets(0, 0, 0);
		ID3D11ShaderResourceView* nullSRV[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		d3d_imm_context_->VSSetShaderResources(0, 8, nullSRV);
		d3d_imm_context_->PSSetShaderResources(0, 8, nullSRV);
		d3d_imm_context_->CSSetShaderResources(0, 8, nullSRV);
		ID3D11UnorderedAccessView *nullUAV[1] = {0};
		d3d_imm_context_->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	}

	void D3DRenderEngine::SetDeferredRenderingState()
	{
		ID3D11DepthStencilState* depth_state= depth_off_->GetDepthStencilState();
		d3d_imm_context_->OMSetDepthStencilState(depth_state, 1);
	}

	void D3DRenderEngine::SetNormalState()
	{
		ID3D11DepthStencilState* depth_state= depth_on_->GetDepthStencilState();
		d3d_imm_context_->OMSetDepthStencilState(depth_state, 1);

		ID3D11RasterizerState* res_state = wire_frame_->GetRasterizerState();
		d3d_imm_context_->RSSetState(res_state);

	}

	void D3DRenderEngine::TrunoffCull()
	{
		ID3D11DepthStencilState* depth_state= cull_depth_->GetDepthStencilState();
		d3d_imm_context_->OMSetDepthStencilState(depth_state, 1);

		ID3D11RasterizerState* res_state = cull_back_->GetRasterizerState();
		d3d_imm_context_->RSSetState(res_state);
	}





}

