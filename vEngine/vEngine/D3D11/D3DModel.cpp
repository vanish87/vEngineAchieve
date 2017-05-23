#include "D3DModel.h"
#include "DirectXTex.h"
#include "D3DRenderEngine.h"

namespace vEngine
{
	D3DModel::D3DModel(void)
	{
	} 


	D3DModel::~D3DModel(void)
	{
	}

	void D3DModel::LoadFile( std::string file_name, CompleteCallBack callback /*= nullptr*/)
	{
		Model::LoadFile(file_name, callback);
		PRINT(file_name + " loaded");
	}

	void D3DModel::SetRenderParameters()
	{
		shader_object_->SetMatrixVariable("g_world_matrix", model_matrix_);
		//TODO : use texture array to store every pom texture of mesh
		if(pom_enabled_)
			shader_object_->SetReource("normal_map_tex", pom_texture_, 1);
	}

	void D3DModel::Render(int pass_index)
	{
		D3DRenderEngine* re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		//for each mesh 
		for(size_t i =0; i < meshes_.size(); i++)
		{
			//set texture
			//set material
			shader_object_->SetRawData("gMaterial", materials_[i], sizeof(Material));		
			float4x4 view_mat = re->CurrentFrameBuffer()->GetViewport().GetCamera().GetViewMatirx();
			float4x4 world_view_inv_transpose = Math::InverTranspose(meshes_[i]->GetModelMatrix()* model_matrix_ * view_mat);
			float4x4 world_inv_transpose = Math::InverTranspose(meshes_[i]->GetModelMatrix()* model_matrix_);
			shader_object_->SetMatrixVariable("g_mwv_inv_transpose", world_view_inv_transpose);
			shader_object_->SetMatrixVariable("g_mw_inv_transpose", world_inv_transpose);
			//set mesh's parameter
			meshes_[i]->SetRenderParameters();
			//set mesh's texture
			Material* mat = materials_[meshes_[i]->GetMaterialID()];
			if(mat->diffuse_tex != 0)
			{
				shader_object_->SetReource("mesh_diffuse",textures_[mat->diffuse_tex-1], 1);
			}
			if(mat->normalmap_tex != 0)
			{
				shader_object_->SetBool("g_normal_map", true);
				//set normal map there
				shader_object_->SetReource("normal_map_tex", textures_[mat->normalmap_tex-1], 1);
			}
			else
			{
				shader_object_->SetBool("g_normal_map", false);
			}
			//render
			meshes_[i]->Render(pass_index);
			//end render
			meshes_[i]->EndRender();
		}		
		
	}

	void D3DModel::EndRender()
	{

	}

	void D3DModel::LoadShaderFile( std::string file_name )
	{
		shader_object_ = new D3DShaderobject();
		D3DShaderobject* d3d_shader_object = static_cast<D3DShaderobject*>(shader_object_);
		d3d_shader_object->LoadFxoFile(file_name);

		//set all meshes' shader file to this one
		for(size_t i =0; i < meshes_.size(); i++)
		{
			meshes_[i]->SetShaderObject(d3d_shader_object);
		}

		//Default init for Model shader
		d3d_shader_object->SetTechnique("ColorTech");
		RenderEngine* render_engine = &Context::Instance().GetRenderFactory().GetRenderEngine();
		if(render_engine->GetRenderSetting().deferred_rendering)
			d3d_shader_object->SetTechnique("GbufferTech");
		d3d_shader_object->SetMatrixVariable("g_world_matrix");
		d3d_shader_object->SetMatrixVariable("g_mwv_inv_transpose");
		d3d_shader_object->SetMatrixVariable("g_mw_inv_transpose");
		d3d_shader_object->SetMatrixVariable("g_view_proj_matrix");
		d3d_shader_object->SetMatrixVariable("g_view_matrix");
		d3d_shader_object->SetMatrixVariable("g_inv_proj_matrix");
		d3d_shader_object->SetMatrixVariable("g_inv_view_matrix");
		d3d_shader_object->SetMatrixVariable("g_model_matrix");
		d3d_shader_object->SetVariable("gMaterial");
		d3d_shader_object->SetShaderResourceVariable("mesh_diffuse");


		d3d_shader_object->SetBool("g_pom_enable");
		d3d_shader_object->SetBool("g_pom_enable", pom_enabled_);
		d3d_shader_object->SetBool("g_normal_map");
		d3d_shader_object->SetShaderResourceVariable("normal_map_tex");

		//d3d_shader_object->SetVectorVariable("g_light_color");
		//d3d_shader_object->SetVectorVariable("g_light_position");
		d3d_shader_object->SetVectorVariable("g_eye_pos");

		d3d_shader_object->SetVariable("light");
		d3d_shader_object->SetMatrixVariable("g_shadow_transform");
		d3d_shader_object->SetMatrixVariable("g_light_view_proj");
		d3d_shader_object->SetMatrixVariable("main_camera_inv_view");
		d3d_shader_object->SetMatrixVariable("main_camera_inv_proj");
		
		
		if(render_engine->GetRenderSetting().deferred_rendering)
		{
			//init parameter here
			d3d_shader_object->SetShaderResourceVariable("diffuse_tex");
			d3d_shader_object->SetShaderResourceVariable("depth_tex");
			d3d_shader_object->SetShaderResourceVariable("normal_tex");
			//d3d_shader_object->SetShaderResourceVariable("position_tex");
			d3d_shader_object->SetShaderResourceVariable("shadow_map_tex");

			//SSDO
			d3d_shader_object->SetShaderResourceVariable("blur_occlusion_tex");

			//lighting buffer
			d3d_shader_object->SetShaderResourceVariable("lighting_tex");
		}
			
	}

	Texture* D3DModel::LoadTexture( std::string file_name )
	{
		if(file_name.empty())return 0;
		//only for load d3d Texture
		//if I have a original texture file loader, remove it, do Texture loading on Model Class
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());	
		ID3D11Resource* texture;
		//TODO: use a resource loader for search path
		std::ifstream ifs(file_name);
		if (!ifs.good())
		{
			std::ifstream ifs1("Media/textures/" + file_name);
			if (ifs1.good())
			{
				file_name = "Media/textures/" + file_name;
			}
			else
			{
				file_name = "Media/" + file_name;
			}
		}
		
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;
		if (file_name.find(".tga") != std::string::npos)
		{
			HRESULT result = DirectX::LoadFromTGAFile(std::wstring(file_name.begin(), file_name.end()).c_str(), &metadata, image);
			if (FAILED(result))
				PRINT("Cannot Load Texture File" + file_name);
		}
		else
		{
			HRESULT result = DirectX::LoadFromWICFile(std::wstring(file_name.begin(), file_name.end()).c_str(), NULL, &metadata, image);
			if (FAILED(result))
				PRINT("Cannot Load Texture File" + file_name);
		}
		HRESULT result = DirectX::CreateTexture(d3d_re->D3DDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
		if(FAILED(result))
			PRINT("Cannot Load Texture File");

		D3D11_RESOURCE_DIMENSION dimension;
		texture->GetType(&dimension);
		CHECK_ASSERT(dimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D);

		Texture* d3d_tex = Context::Instance().GetRenderFactory().MakeTexture2D(texture);
		
		return d3d_tex;
		
	}

	void D3DModel::LoadPomTexture( std::string file_name )
	{
		//TODO: write a add material fun to add pom texture
		pom_enabled_ = true;
		pom_texture_ = LoadTexture(file_name);
		//pom_srv_ = Context::Instance().GetRenderFactory().MakeRenderBuffer(pom_texture_, AT_GPU_READ_WRITE, BU_SHADER_RES);
	}


}
