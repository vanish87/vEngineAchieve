#include "Engine/Header/GameObject.h"

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
		//see Component pattern in Game Programming Pattern
		//here we should have to update all components of game objects
	}

	void GameObject::Load()
	{

	}

	std::string GameObject::GetName()
	{
		//should never call this
		CHECK_AND_ASSERT(false,"should never call this");
		return "ERROR";
	}

	void GameObject::AddComponent(GameObject* const GameObject_)
	{
		CHECK_AND_ASSERT(GameObject_ != nullptr, "GameObject Error");
		CHECK_ASSERT(*GameObject_ != GameObject::NullObject());
		CHECK_ASSERT(GameObject_ != this);
		this->ConponentList_[GameObject_->id()] = GameObject_;
		CHECK_AND_ASSERT(this->ConponentList_[GameObject_->id()] == GameObject_, "GameObject Error");
	}

	GameObject& GameObject::FindComponentByUUID(const UUID& UUID_)
	{
		if(this->ConponentList_.find(UUID_) == this->ConponentList_.end()) return GameObject::NullObject();
		GameObject* Ret = this->ConponentList_[UUID_];
		CHECK_AND_ASSERT(Ret != nullptr, "Find Error");
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
