#include "D3DSkyDome.h"

#include "D3DRenderEngine.h"
#include "D3DTexture.h"
#include "DirectXTex.h"
#include "Engine/Header/RenderTools.h"

namespace vEngine
{

	D3DSkyDome::D3DSkyDome(std::string file_name )
	{
		cube_texture_ = LoadTexture(file_name);
		
		float4x4 model_matrix;
		Math::Identity(model_matrix);
		sky_mesh_ = RenderTools::GetInstance().MakeSphereMesh();

		local_matrix_ = model_matrix;
	}


	D3DSkyDome::~D3DSkyDome(void)
	{
	}

	Texture* D3DSkyDome::LoadTexture( std::string file_name )
	{
		if(file_name.empty())return 0;
		//only for load d3d Texture
		//if I have a original texture file loader, remove it, do Texture loading on Model Class
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());	
		ID3D11Resource* texture;
		std::wstring widestr = std::wstring(file_name.begin(), file_name.end());
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;
		DirectX::LoadFromDDSFile(widestr.c_str(), NULL, &metadata, image);
		HRESULT result = DirectX::CreateTexture(d3d_re->D3DDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
		if(FAILED(result))
			PRINT_ERROR("Cannot Load Texture File");

		D3D11_RESOURCE_DIMENSION dimension;
		texture->GetType(&dimension);
		CHECK_ASSERT(dimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D);

		Texture* d3d_tex = Context::Instance().GetRenderFactory().MakeTexture2D(texture);

		return d3d_tex;
	}

	void D3DSkyDome::LoadShaderFile( std::string file_name )
	{
		//shader_object_ = Context::Instance().GetRenderFactory().MakeShaderObject();
		//D3DShaderobject* d3d_shader_object = static_cast<D3DShaderobject*>(shader_object_);
		//d3d_shader_object->LoadBinaryFile(file_name);

		shader_object_ = ShaderObject::FindShaderByName(file_name);

		//Default init for SkyDome shader
		shader_object_->SetTechnique("SkyDomeTech");

		shader_object_->SetShaderResourceVariable("background_tex");
		shader_object_->SetMatrixVariable("g_world_matrix");
		shader_object_->SetMatrixVariable("g_view_proj_matrix");
		shader_object_->SetMatrixVariable("g_view_matrix");
		shader_object_->SetMatrixVariable("g_inv_proj_matrix");
		shader_object_->SetMatrixVariable("g_inv_view_matrix");
		shader_object_->SetMatrixVariable("g_model_matrix");
		
		shader_object_->SetVectorVariable("g_eye_pos");

		sky_mesh_->SetShaderObject(shader_object_);
	}

	void D3DSkyDome::Render( int pass_index )
	{
		float4x4 root;
		Math::Identity(root);
		sky_mesh_->SetRenderParameters(root);
		sky_mesh_->Render(pass_index);
		sky_mesh_->EndRender();
	}

	void D3DSkyDome::SetRenderParameters()
	{

		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());	
		D3DShaderobject* d3d_shader_object = static_cast<D3DShaderobject*>(shader_object_);
		float3 cam_pos = d3d_re->CurrentFrameBuffer()->GetViewport().GetCamera().GetPos();
		Math::Translate(this->local_matrix_, cam_pos.x(), cam_pos.y(), cam_pos.z());
		d3d_shader_object->SetMatrixVariable("g_world_matrix", this->local_matrix_);
		d3d_re->TrunoffCull();
		//RenderBuffer* cude_srv = Context::Instance().GetRenderFactory().MakeRenderBuffer(cube_texture_, AT_GPU_READ_WRITE, BU_SHADER_RES); 
		shader_object_->SetReource("background_tex", cube_texture_);
		//throw std::exception("The method or operation is not implemented.");
	}

	void D3DSkyDome::EndRender()
	{
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());	
		d3d_re->SetNormalState();

		//throw std::exception("The method or operation is not implemented.");
	}


	



}
