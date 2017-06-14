#ifndef STRING_HASH_H_
#define STRING_HASH_H_

#pragma once
#include "Engine/Header/CommonTools.h"
#include <string>

namespace vEngine
{	
	//runtime string hash for debug only
	class StringHash
	{
	public:
		StringHash(const std::string String)
			:HashValue(_Hash(String.c_str(), 0)), StringValue(String)
		{

		}

		StringHash()
			:HashValue(0), StringValue("NULL")
		{

		}

		bool operator==(const StringHash & rhs) const
		{
			return  this->HashValue == rhs.HashValue;
		}

		StringHash& operator=(const StringHash & rhs)
		{
			if (this == &rhs) return *this;
			this->HashValue = rhs.HashValue;
			this->StringValue = rhs.StringValue;
			return *this;
		}
	private:
		size_t HashValue;
		std::string StringValue;
	};
}

#endif