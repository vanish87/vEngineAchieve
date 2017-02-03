#include "Engine\Header\GameObject.h"

namespace vEngine
{
	GameObject::GameObject(void)
	{
		DebugTracking::GetInstance().Track(this);
	}

	GameObject::~GameObject(void)
	{
		DebugTracking::GetInstance().UnTrack(this);
	}

	void GameObject::Update()
	{

	}

	void GameObject::Load()
	{

	}

	std::string GameObject::GetName()
	{
		//should never call this
		assert(false);
		return "ERROR";
	}

	void GameObject::AddComponent(GameObject* const GameObject_)
	{
		assert(GameObject_ != nullptr);
		assert(GameObject_ != &GameObject::NullObject() && GameObject_ != this);
		this->ConponentList_[GameObject_->id()] = GameObject_;
		assert(this->ConponentList_[GameObject_->id()] == GameObject_);
	}

	GameObject& GameObject::FindComponentByUUID(const UUID& UUID_)
	{
		if(this->ConponentList_.find(UUID_) == this->ConponentList_.end()) return GameObject::NullObject();
		GameObject* Ret = this->ConponentList_[UUID_];
		assert(Ret != nullptr);
		return *Ret;
	}


	GameObject& GameObject::NullObject()
	{
		static NGameObject Instance;
		return Instance;
	}

	NGameObject::NGameObject()
		:GameObject()
	{

	}

	NGameObject::~NGameObject()
	{

	}

}
