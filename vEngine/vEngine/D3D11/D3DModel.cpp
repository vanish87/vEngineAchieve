#include "D3DModel.h"
#include "DirectXTex.h"

namespace vEngine
{
	D3DModel::D3DModel(void)
	{
	} 


	D3DModel::~D3DModel(void)
	{
	}

	void D3DModel::LoadFile( std::string file_name )
	{
		Model::LoadFile(file_name);
		PRINT(file_name + " loaded");
	}

	void D3DModel::SetRenderParameters()
	{
		D3DShaderobject* d3d_shader_object = static_cast<D3DShaderobject*>(shader_object_);
		d3d_shader_object->SetMatrixVariable("g_world_matrix", model_matrix_);

		//TODO : use texture array to store every pom texture of mesh
		if(pom_enabled_)
			d3d_shader_object->SetReource("normal_map_tex", pom_srv_, 1);
		//if(deferred_rendering)
			//set parameter here
			//
	}

	void D3DModel::Render(int pass_index)
	{
		//TODO : After write a normal ShaderObject, move these to SceneManager->Flush(), because all Render_elenment shader the same lights.
		//set light parameter
		//std::vector<Light*> lights = Context::Instance().GetSceneManager().GetLights();
		//D3DRenderBuffer* lights_buffer = static_cast<D3DRenderBuffer*>(Context::Instance().GetRenderFactory().GetRenderEngine().GetLightsBuufer());
		//shader_object_->SetReource("gLight", lights_buffer, 0);

		//for each mesh 
		for(size_t i =0; i < meshes_.size(); i++)
		{
			//set texture
			//set material
			shader_object_->SetRawData("gMaterial", materials_[i], sizeof(Material));		
			Camera* camera = Context::Instance().GetRenderFactory().GetRenderEngine().CurrentFrameBuffer()->GetFrameCamera();
			float4x4 view_mat = camera->GetViewMatirx();
			float4x4 world_inv_transpose = Math::InverTranspose( meshes_[i]->GetModelMatrix()* model_matrix_ * view_mat);
			shader_object_->SetMatrixVariable("g_mwv_inv_transpose", world_inv_transpose);
			//set mesh's parameter
			meshes_[i]->SetRenderParameters();
			//set mesh's texture
			Material* mat = materials_[meshes_[i]->GetMaterialID()];
			if(mat->diffuse_tex != 0)
			{
				shader_object_->SetReource("mesh_diffuse",tex_srvs_[mat->diffuse_tex-1], 1);
			}
			if(mat->normalmap_tex != 0)
			{
				shader_object_->SetBool("g_normal_map", true);
				//set normal map there
				shader_object_->SetReource("normal_map_tex",tex_srvs_[mat->normalmap_tex-1], 1);
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
		std::ifstream ifs(file_name);
		if (!ifs.good())
		{
			file_name = "Media/" + file_name;
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
		HRESULT result = DirectX::CreateTexture(d3d_re->D3DDevice(), image.GetImages(), image.GetImageCount(), metadata,&texture);
		if(FAILED(result))
			PRINT("Cannot Load Texture File");
		ID3D11Texture2D* texture_2d= static_cast<ID3D11Texture2D*>(texture);
		D3D11_TEXTURE2D_DESC desc;
		texture_2d->GetDesc(&desc);
		//TODO: use unified routine to Create Texture
		D3DTexture2D* d3d_tex = new D3DTexture2D(desc,texture_2d, TEXTURE2D);
		
		return d3d_tex;
		
	}

	void D3DModel::LoadPomTexture( std::string file_name )
	{
		//TODO: write a add material fun to add pom texture
		pom_enabled_ = true;
		pom_texture_ = LoadTexture(file_name);
		pom_srv_ = Context::Instance().GetRenderFactory().MakeRenderBuffer(pom_texture_, AT_GPU_READ_WRITE, BU_SHADER_RES);
	}


}
