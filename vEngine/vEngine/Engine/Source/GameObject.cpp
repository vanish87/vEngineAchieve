#include "Engine\Header\GameObject.h"

namespace MocapGE
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
