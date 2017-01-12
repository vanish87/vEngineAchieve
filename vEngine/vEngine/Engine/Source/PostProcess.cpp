#include "Engine\Header\PostProcess.h"
#include "Engine\Header\Context.h"

namespace MocapGE
{
	PostProcess::PostProcess(void)
	{
		Configure::RenderSetting render_setting =  Context::Instance().GetRenderFactory().GetRenderEngine().GetRenderSetting();
		output_buffer_ = Context::Instance().GetRenderFactory().MakeFrameBuffer(render_setting);
		//make a full screen qua for lighting pass
		VertexType* vb = new VertexType[6];
		uint32_t* ib = new uint32_t[6];
		vb[0].position = float3(-1, +1, 1);
		vb[1].position = float3(+1, +1, 1);
		vb[2].position = float3(-1, -1, 1);
		vb[3].position = float3(+1, -1, 1);
		vb[4].position = float3(-1, -1, 1);
		vb[5].position = float3(+1, +1, 1);

		ib[0] = 0;
		ib[1] = 1;
		ib[2] = 2;
		ib[3] = 3;
		ib[4] = 4;
		ib[5] = 5;

		//call MakeRenderLayout
		RenderLayout* render_layout = Context::Instance().GetRenderFactory().MakeRenderLayout();
		//call MakeRenderBuffer(Vertex)
		InitData init_data;
		init_data.data = vb;
		init_data.row_pitch = 0;
		init_data.slice_pitch = 0;
		RenderBuffer* vertex_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ, BU_VERTEX, 6 ,sizeof(VertexType));
		//delete[] vb;
		//call MakeRenderBuffer(Index)
		init_data.data = ib;
		init_data.row_pitch = 0;
		init_data.slice_pitch = 0;
		RenderBuffer* index_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ, BU_INDEX, 6, sizeof(uint32_t));
		//delete[] ib;

		//add VertexBuffer to renderlayout;
		render_layout->AddBuffer(vertex_buffer, sizeof(VertexType));
		//add IndexBuffer to renderlayout;
		render_layout->AddBuffer(index_buffer, 6);
		//set Primitivetype of renderlayout;
		render_layout->SetPrimitive(PT_TRIANGLELIST);
		//set Input layout Semi
		std::vector<VertexUsage> inputlayout;
		inputlayout.push_back(VU_POSITION);
		render_layout->SetInputLayout(inputlayout);
		float4x4 model_matrix;
		Math::Identity(model_matrix);
		fullscreen_mesh_ = new MocapGE::Mesh("full screen quad", render_layout, model_matrix, vb, 6, ib);
	}


	PostProcess::~PostProcess(void)
	{
	} 

	void PostProcess::LoadPPFile( std::string file_name )
	{
		
	}

	void PostProcess::SetInput( Texture* tex, size_t index )
	{

		RenderBuffer* shader_resource = Context::Instance().GetRenderFactory().MakeRenderBuffer(tex, AT_GPU_READ, BU_SHADER_RES);
		if(index >= input_srv_.size())
		{
			//Add to input
			input_srv_.push_back(shader_resource);
			fullscreen_mesh_->GetShaderObject()->SetShaderResourceVariable("input_tex_" + std::to_string(static_cast<long long>(input_srv_.size() - 1)));
		}
		else
		{
			input_srv_[index] = shader_resource;
		}
	}

	void PostProcess::SetOutput( Texture* tex, size_t index )
	{
		RenderView* render_view = Context::Instance().GetRenderFactory().MakeRenderView(tex, 1, 0);
		output_buffer_->AddRenderView(render_view);
	}

	void PostProcess::Apply()
	{
		RenderEngine* re = &Context::Instance().GetRenderFactory().GetRenderEngine();
		re->SetDeferredRenderingState();
		re->BindFrameBuffer(output_buffer_);
		Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameBegin();
		ShaderObject* shander_object = fullscreen_mesh_->GetShaderObject();
		for(size_t i = 0; i < input_srv_.size(); ++i)
		{
			//TODO: temp solution
			shander_object->SetReource("input_tex_" + std::to_string(static_cast<long long>(i)), input_srv_[i], 1);
		}
		fullscreen_mesh_->SetRenderParameters();
		fullscreen_mesh_->Render(0);
		fullscreen_mesh_->EndRender();
		re->SetNormalState();
		Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameEnd();
	}

	void PostProcess::SetPPShader( ShaderObject* shander_object )
	{
		shander_object->SetMatrixVariable("g_model_matrix");
		shander_object->SetMatrixVariable("g_world_matrix");
		shander_object->SetMatrixVariable("g_mwv_inv_transpose");
		shander_object->SetMatrixVariable("g_view_proj_matrix");
		shander_object->SetMatrixVariable("g_view_matrix");
		shander_object->SetMatrixVariable("g_inv_proj_matrix");
		shander_object->SetMatrixVariable("g_inv_view_matrix");
		shander_object->SetVectorVariable("g_eye_pos");
		shander_object->SetVectorVariable("g_eye_z");

		shander_object->SetShaderResourceVariable("input_tex_0");
		shander_object->SetShaderResourceVariable("input_tex_1");
		shander_object->SetShaderResourceVariable("input_tex_2");
		//shander_object->SetVariable("gMaterial");
		//shander_object->SetShaderResourceVariable("mesh_diffuse");
		fullscreen_mesh_->SetShaderObject(shander_object);

	}

	void PostProcess::SetCamera( Camera* camera )
	{
		output_buffer_->SetFrameCamera(camera);
	}

}

