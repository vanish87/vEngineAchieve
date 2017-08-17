#ifndef UUID_H_
#define UUID_H_

#pragma once
#include "Common/Header/CommonPreDec.h"

namespace vEngine
{
	//very simple and intuitive uuid generate
	class UUIDGenerator
	{
	private:
		explicit UUIDGenerator()
		: currentid_(0)	{};
		UUIDGenerator(const UUIDGenerator& rhs) {};
		UUIDGenerator& operator=(const UUIDGenerator& rhs) { return *this; };
		virtual ~UUIDGenerator() {};

		uint64_t currentid_;
	public:
		static UUIDGenerator& GetInstance()
		{
			static UUIDGenerator Instance;
			return Instance;
		}
		uint64_t Generate()
		{
			return ++this->currentid_;
		}
	};

	class UUID
	{
		friend struct std::hash<vEngine::UUID>;
		friend class GameObject;
		friend class DebugTracking;
	public:
		virtual ~UUID(void) {};

	private:
		UUID()
		:data_(0)
		{
			data_ = UUIDGenerator::GetInstance().Generate();
		}
	private:
		explicit UUID(uint64_t uuid)
		{

		}
	public:
		UUID(const UUID & rhs)//copy constructor
		{
			this->data_ = rhs.data_;
		}
				
		UUID& operator=(const UUID & rhs)
		{
			if (this == &rhs) return *this;
			data_ = rhs.data_;
			return *this;
		}
		//compare operator
		bool operator==(const UUID & rhs) const
		{
			return  this->data_ == rhs.data_;
		}
		bool operator!=(const UUID & rhs) const
		{
			return  this->data_ != rhs.data_;
		}
	private:
		uint64_t data_;
	};
}

namespace std
{
	template <>
	struct hash<vEngine::UUID>
	{
		vEngine::uint64_t operator()(vEngine::UUID const& value) const
		{
			 hash<vEngine::uint64_t> HashFunc;
			 return HashFunc(value.data_);
		}
	};
}

#endif
