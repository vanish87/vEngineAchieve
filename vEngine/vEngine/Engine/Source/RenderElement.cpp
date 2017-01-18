#include "Engine\Header\RenderElement.h"

namespace vEngine
{
	RenderElement::RenderElement(void)
	{
	}


	RenderElement::~RenderElement(void)
	{
	}

	ShaderObject* RenderElement::GetShaderObject()
	{
		return shader_object_;
	}

	void RenderElement::SetShaderObject( ShaderObject* shader_object )
	{
		this->shader_object_ = shader_object;
	}

	void RenderElement::LoadShaderFile( std::string file_name )
	{
		//load basic shader file for render element
		
	}

	float4x4 & RenderElement::GetModelMatrix()
	{
		return model_matrix_;
	}

	void RenderElement::SetModelMatrix( float4x4 model_matrix )
	{
		this->model_matrix_ = model_matrix;
	}


}