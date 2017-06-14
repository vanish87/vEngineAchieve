#include "Engine/Header/ShaderObject.h"
#include <unordered_map>
#include "Engine/Header/Context.h"

namespace vEngine
{
	static std::unordered_map<std::string, ShaderObject*> kShaderMaps;

	ShaderObject::ShaderObject(void)
	{
	}


	ShaderObject::~ShaderObject(void)
	{
	}

	void ShaderObject::LoadBinaryFile(std::string file_name)
	{
		CHECK_ASSERT(false);
	}

	ShaderObject* ShaderObject::FindShaderByName(const std::string shader_name)
	{
		if (kShaderMaps.find(shader_name) == kShaderMaps.end())
		{
			kShaderMaps[shader_name] = Context::Instance().GetRenderFactory().MakeShaderObject();
			kShaderMaps[shader_name]->LoadBinaryFile("FxFiles/"+ shader_name+".cso");
			kShaderMaps[shader_name]->SetupShaderVariable(shader_name);
		}

		return kShaderMaps[shader_name];
	}

	void ShaderObject::SetupShaderVariable(const std::string shader_name)
	{
		this->SetupCommonVariable();
		if (shader_name == "DeferredLighting")
		{
			this->SetMatrixVariable("g_model_matrix");
			this->SetMatrixVariable("g_view_proj_matrix");
			this->SetMatrixVariable("g_m_inv_transpose");

			this->SetVariable("gMaterial");
			this->SetMatrixVariable("g_light_view_proj");
			this->SetMatrixVariable("main_camera_inv_view");
			this->SetMatrixVariable("main_camera_inv_proj");

			this->SetVariable("light");
			this->SetBool("g_normal_map");
			this->SetShaderResourceVariable("normal_map_tex");


			//init parameter here
			this->SetShaderResourceVariable("diffuse_tex");
			this->SetShaderResourceVariable("depth_tex");
			this->SetShaderResourceVariable("position_tex");
			this->SetShaderResourceVariable("normal_tex");
			//this->SetShaderResourceVariable("position_tex");
			this->SetShaderResourceVariable("shadow_map_tex");

			//SSDO
			this->SetShaderResourceVariable("blur_occlusion_tex");

			//lighting buffer
			this->SetShaderResourceVariable("lighting_tex"); 

			//mesh diffuse texture
			this->SetShaderResourceVariable("mesh_diffuse");
			
		}
	}

	void ShaderObject::SetupCommonVariable()
	{		
		this->SetMatrixVariable("g_model_matrix");
		this->SetMatrixVariable("g_view_proj_matrix");
		this->SetVectorVariable("g_eye_pos");
	}

}
