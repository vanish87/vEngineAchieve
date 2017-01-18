#include "Engine\Header\GameObject.h"

namespace vEngine
{
	GameObject::GameObject(void)
	{
		DebugTracking::GetInstace().Track(this);
	}

	GameObject::~GameObject(void)
	{
		DebugTracking::GetInstace().UnTrack(this);
	}

	void GameObject::Update()
	{

	}

	std::string GameObject::GetName()
	{
		//should never call this
		//assert(false);
		return "ERROR";
	}

	void GameObject::AddComponent(const GameObject& GameObject_)
	{
		assert(GameObject_ != GameObject::NullObject() && GameObject_.id() != this->id());
		this->ConponentList_[GameObject_.id()] = GameObject_;
		assert(this->ConponentList_[GameObject_.id()] == GameObject_);
	}

	GameObject& GameObject::FindComponentByUUID(const UUID& UUID_)
	{
		if(this->ConponentList_.find(UUID_) == this->ConponentList_.end()) return GameObject::NullObject();
		return this->ConponentList_[UUID_];
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
