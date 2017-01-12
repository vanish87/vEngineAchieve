#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#pragma once
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

		static GameObject& NullObject();

	protected:
		UUID id;
	};

	class NGameObject : public GameObject
	{
	public:
		NGameObject();
		~NGameObject();
	};

}

#endif

