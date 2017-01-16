#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#pragma once
#include <vector>
#include "Common\Header\CommonPreDec.h"
#include "Common\Header\UUID.h"

namespace MocapGE
{
	class GameObject
	{
	public:
		GameObject(void);
		virtual ~GameObject(void);

		virtual void Update();
		virtual void AddComponent(const GameObject& GameObject_);

		static GameObject& NullObject();

	protected:
		UUID id;

		std::vector<GameObject> ConponentList;
	};

	class NGameObject : public GameObject
	{
	public:
		NGameObject();
		~NGameObject();
	};

}

#endif

