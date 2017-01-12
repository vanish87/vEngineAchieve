#include "SceneObject.h"
#include "Context.h"

namespace MocapGE
{
	SceneObject::SceneObject(void)
	{
	}


	SceneObject::~SceneObject(void)
	{
	}

	void SceneObject::Update()
	{
		 this->model_matrix_ = render_element_->model_matrix_;
	}

	RenderElement* & SceneObject::GetRenderElement()
	{
		return render_element_;
	}

	bool SceneObject::Visiable()
	{
		return visiable_;
	}

	void SceneObject::SetVisiable( bool visiable )
	{
		visiable_ = visiable;
	}

	void SceneObject::AddToScene()
	{
		Context::Instance().GetSceneManager().AddSceneObject(this);
	}

}
