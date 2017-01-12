#include "GameObject.h"

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
