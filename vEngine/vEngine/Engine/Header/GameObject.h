#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#pragma once
#include <unordered_map>
#include <typeinfo>
#include "Common/Header/CommonPreDec.h"
#include "Engine/Header/EnginePreDec.h"
#include "Common/Header/UUID.h"

namespace vEngine
{
	class GameObject
	{
	public:
		GameObject(void);
		virtual ~GameObject(void);

		virtual void Update();
		virtual void Load();
		//user should keep tracking and make sure GameObject* is valid
		//it could be released by registering an observer to it 
		//and to remove it when it has be destructed.
		virtual void AddComponent(GameObject* const GameObject_);
		virtual GameObject& FindComponentByUUID(const UUID& UUID_);

		//virtual void Serialize();
		//virtual void UnSerialize();

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
		std::unordered_map<UUID, GameObject*> ConponentList_;
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
		std::unordered_map<UUID, GameObject*> ReleasedList_;
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
				//check ReleasedList_ first to ensure it is not released before
				CHECK_ASSERT(this->ReleasedList_.find(obj->first) == this->ReleasedList_.end());
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
			CHECK_ASSERT(this->ReleasedList_.find(object->id()) == this->ReleasedList_.end());
			this->ReleasedList_[object->id()] = object;

			if (this->ConponentList_.find(object->id()) != this->ConponentList_.end())
			{
				this->ConponentList_.erase(object->id());
			}
			//PRINT("Tracking " << typeid(*object).name());
			//PRINT("subclass "<< object->GetName().c_str());
		};
		
	};

}

#endif

