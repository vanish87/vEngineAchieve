#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#pragma once
#include <unordered_map>
#include <typeinfo>
#include "Common\Header\CommonPreDec.h"
#include "Engine\Header\EnginePreDec.h"
#include "Common\Header\UUID.h"

namespace vEngine
{
	class GameObject
	{
	public:
		GameObject(void);
		virtual ~GameObject(void);

		virtual void Update();
		virtual void Load();
		virtual void AddComponent(GameObjectSharedPtr const GameObject_);
		virtual GameObject& FindComponentByUUID(const UUID& UUID_);

		virtual std::string GetName();

		static GameObject& NullObject();

		bool operator==(const GameObject& rhs) const
		{
			return this->id_ == rhs.id_;
		}
		bool operator!=(const GameObject& rhs) const
		{
			return this->id_ != rhs.id_;
		}
		GameObject(const GameObject & rhs)//copy constructor
		{
			this->id_ = rhs.id();
			this->ConponentList_ = rhs.ConponentList_;
		}
		GameObject& operator=(const GameObject & rhs)
		{
			if (this == &rhs) return *this;
			this->id_ = rhs.id();
			this->ConponentList_ = rhs.ConponentList_;
			return *this;
		}

		const UUID& id() const
		{
			return this->id_;
		}

	protected:
		UUID id_;

		std::unordered_map<UUID, GameObjectSharedPtr> ConponentList_;
	};

	class NGameObject : public GameObject
	{
	public:
		NGameObject();
		~NGameObject();

		DEBUG_CLASS_NAME("NULL_OBJECT")
	};

	class DebugTracking
	{
	private:
		DebugTracking() 
		{

		};
		~DebugTracking()
		{

		};

		std::unordered_map<UUID, GameObject*> ConponentList_;
	public:
		static DebugTracking& GetInstance()
		{
			static DebugTracking Instance;
			return Instance;
		};

		void PrintALL()
		{
			for (std::unordered_map<UUID, GameObject*>::iterator obj = ConponentList_.begin(); obj != ConponentList_.end();obj++)
			{
				PRINT("subclass "<<obj->first.data_<< " with "<< obj->second->GetName().c_str());
			}
		}

		void Track(GameObject* const object)
		{
			this->ConponentList_[object->id()] = object;
			//PRINT("Tracking " << typeid(*object).name());
			//PRINT("subclass "<< object->GetName().c_str());
		};
		void UnTrack(GameObject* const object)
		{
			this->ConponentList_.erase(object->id());
			//PRINT("Tracking " << typeid(*object).name());
			//PRINT("subclass "<< object->GetName().c_str());
		};
		
	};

}

#endif

