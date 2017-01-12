#ifndef UUID_H_
#define UUID_H_

#pragma once
#include "Common\Header\CommonPreDec.h"

namespace MocapGE
{
	//very simple and intuitive uuid generate
	class UUIDGenerator
	{
	private:
		explicit UUIDGenerator()
		: currentid_(0)
		{

		};
		~UUIDGenerator() {};

		uint64_t currentid_;
	public:
		static UUIDGenerator& GetInstance()
		{
			static UUIDGenerator Instance;
			return Instance;
		}
		uint64_t Generate()
		{
			return ++currentid_;
		}
	};

	class UUID
	{
	public:
		~UUID(void) {};

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
	private:
		uint64_t data_;
	};
}

#endif
