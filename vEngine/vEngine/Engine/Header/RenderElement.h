#ifndef RENDERELEMENT_H_
#define RENDERELEMENT_H_

#pragma once
#include "EnginePreDec.h"
#include "Common\Header\Matrix.h"
#include "ShaderObject.h"

namespace MocapGE
{
	class RenderElement
	{
		friend class SceneObject;
	public:
		RenderElement(void);
		RenderElement(float4x4 model_matrix)
			:model_matrix_(model_matrix)
		{
		
		};
		~RenderElement(void);

		//virtual void AddToScene() = 0;
		virtual RenderLayout* GetRenderLayout() = 0;

		ShaderObject* GetShaderObject();
		float4x4 & GetModelMatrix();
		void SetShaderObject(ShaderObject* shader_object);
		void SetModelMatrix(float4x4 model_matrix);
		virtual void LoadShaderFile(std::string file_name);

		virtual void SetRenderParameters() = 0;
		virtual void Render(int pass_index) = 0;
		virtual void EndRender() = 0;

	protected:
		float4x4 model_matrix_;

		ShaderObject* shader_object_;

	};

}

#endif

