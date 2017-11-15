#include "Engine/Header/RenderElement.h"
#include "Common/Header/Math.h"

namespace vEngine
{
	RenderElement::RenderElement(void)
	{
		Math::Identity(this->local_matrix_);
	}


	RenderElement::~RenderElement(void)
	{
	}

	ShaderObject* RenderElement::GetShaderObject()
	{
		return shader_object_;
	}

	void RenderElement::SetShaderObject(ShaderObject* shader_object )
	{
		this->shader_object_ = shader_object;
	}

	void RenderElement::LoadShaderFile( std::string file_name )
	{
		//load basic shader file for render element
		
	}

	void RenderElement::SetRenderParameters(const float4x4& parent)
	{
		shader_object_->SetMatrixVariable("g_model_matrix", this->local_matrix_ * parent);
	}

	float4x4 & RenderElement::GetLocalMatrix()
	{
		return local_matrix_;
	}

	void RenderElement::SetModelMatrix( float4x4 model_matrix )
	{
		this->local_matrix_ = model_matrix;
	}


}