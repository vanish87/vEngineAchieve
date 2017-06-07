#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

#pragma once
#include "Common\Header\CommonPreDec.h"
#include "Engine\Header\EnginePreDec.h"
#include "Engine\Header\GameObject.h"
#include "Engine\Header\RenderElement.h"

namespace vEngine
{
	class SceneObject : public GameObject
	{
	public:
		SceneObject(void);
		SceneObject(RenderElement* render_element)
			:render_element_(render_element), local_matrix_(render_element->local_matrix_), visiable_(true)
		{
			
		};
		~SceneObject(void);

		DEBUG_CLASS_FILE_NAME

		//do update aabb and set model_matrix equals to its render_element_'s
		virtual void Update();
		virtual RenderElement* GetRenderElement();

		virtual void AddToScene();

		bool Visiable();
		void SetVisiable(bool visiable);

	protected:

		RenderElement* render_element_;
		float4x4 local_matrix_;

		bool visiable_;
	};

}

#endif

