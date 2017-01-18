#include "Engine\Header\GameObject.h"

namespace vEngine
{
	static UUID ZERO_UUID;
	GameObject::GameObject(void)
		:id(ZERO_UUID)
	{
	}


	GameObject::~GameObject(void)
	{
	}

	void GameObject::Update()
	{

	}	

	void GameObject::AddComponent(const GameObject& GameObject_)
	{
		assert(GameObject_ != GameObject::NullObject());
		this->ConponentList.push_back(GameObject_);
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
