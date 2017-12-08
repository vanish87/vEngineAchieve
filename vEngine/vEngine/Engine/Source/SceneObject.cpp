#include "Engine/Header/SceneObject.h"
#include "Engine/Header/Context.h"

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
		GameObject::Update();

		CHECK_ASSERT(this->render_element_ != nullptr);
		if (this->render_element_ != nullptr)
		{
			this->local_matrix_ = render_element_->local_matrix_;
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

	bool SceneObject::MainThreadUpdate()
	{
		return this->update_in_main_thread_;
	}

	void SceneObject::SetMainThreadUpdate(bool InMain)
	{
		this->update_in_main_thread_ = InMain;
	}

	void SceneObject::AddToScene()
	{
		Context::Instance().GetSceneManager().AddSceneObject(this);
	}

}
