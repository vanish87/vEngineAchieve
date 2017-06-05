#include "Engine\Header\SceneObject.h"
#include "Engine\Header\Context.h"

namespace vEngine
{
	SceneObject::SceneObject(void)
		:render_element_(nullptr)
	{
	}


	SceneObject::~SceneObject(void)
	{
	}

	void SceneObject::Update()
	{
		CHECK_ASSERT(this->render_element_ != nullptr);
		if (this->render_element_ != nullptr)
		{
			this->model_matrix_ = render_element_->model_matrix_;
		}
	}

	RenderElement* SceneObject::GetRenderElement()
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
