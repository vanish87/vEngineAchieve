#ifndef RENDERELEMENT_H_
#define RENDERELEMENT_H_

#pragma once
#include "Common/Header/Matrix.h"
#include "Engine/Header/EnginePreDec.h"
#include "Engine/Header/ShaderObject.h"
#include "Engine/Header/GameObject.h"

namespace vEngine
{
	class RenderElement : public GameObject
	{
		friend class SceneObject;
	public:
		RenderElement(void);
		RenderElement(float4x4 local_matrix)
			:local_matrix_(local_matrix)
		{
		
		};
		~RenderElement(void);

		//virtual void AddToScene() = 0;
		virtual RenderLayout* GetRenderLayout() = 0;

		ShaderObject* GetShaderObject();
		float4x4 & GetLocalMatrix();
		void SetShaderObject(ShaderObject* shader_object);
		void SetModelMatrix(float4x4 model_matrix);
		virtual void LoadShaderFile(std::string file_name);

		virtual void SetRenderParameters(const float4x4& parent) = 0;
		virtual void Render(int pass_index) = 0;
		virtual void EndRender() = 0;

	protected:
		float4x4 local_matrix_;

		ShaderObject* shader_object_;

	};

}

#endif

