#include "Engine\Header\PostProcess.h"
#include "Engine\Header\Context.h"
#include "Engine\Header\RenderTools.h"

namespace vEngine
{
	PostProcess::PostProcess(void)
	{
		Configure::RenderSetting render_setting =  Context::Instance().GetRenderFactory().GetRenderEngine().GetRenderSetting();
		output_buffer_ = Context::Instance().GetRenderFactory().MakeFrameBuffer(render_setting.width,render_setting.height);
		//make a full screen qua for lighting pass
		fullscreen_mesh_ = RenderTools::GetInstance().MakeFullScreenMesh();
	}


	PostProcess::~PostProcess(void)
	{
	} 

	void PostProcess::LoadPPFile( std::string file_name )
	{
		
	}

	void PostProcess::SetInput( Texture* tex, size_t index )
	{

		RenderBuffer* shader_resource = Context::Instance().GetRenderFactory().MakeRenderBuffer(tex, AT_GPU_READ_WRITE, BU_SHADER_RES);
		if(index >= input_srv_.size())
		{
			//Add to input
			input_srv_.push_back(shader_resource);
			fullscreen_mesh_->GetShaderObject()->SetShaderResourceVariable("input_tex_" + std::to_string(static_cast<long long>(input_srv_.size() - 1)));
		}
		else
		{
			//PRINT_AND_ASSERT("Memory leak here");
			RenderBuffer* old = input_srv_[index];
			delete old;
			input_srv_[index] = shader_resource;
		}
	}

	void PostProcess::SetOutput( Texture* tex, size_t index )
	{
		RenderView* render_view = Context::Instance().GetRenderFactory().MakeRenderView(tex, 1, 0);
		if (output_buffer_->GetViewport().Height() != render_view->Height()
			|| output_buffer_->GetViewport().Width() != render_view->Width()
			)
		{
			delete this->output_buffer_;
			output_buffer_ = Context::Instance().GetRenderFactory().MakeFrameBuffer(render_view->Width(), render_view->Height());
		}
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
		output_buffer_->GetViewport().SetCamera(camera);
	}

}

