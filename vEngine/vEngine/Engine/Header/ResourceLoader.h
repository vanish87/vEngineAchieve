#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#pragma once
#include <queue>
#include "Common\Header\CommonPreDec.h"

namespace vEngine
{
	class ResourceLoader
	{
		SINGLETON_PRAVATE(ResourceLoader)

	public:
		static ResourceLoader& GetInstance()
		{
			static ResourceLoader Instance;
			return Instance;
		};

		void AddAsync();
		void AddSync();
	private:
		//std::queue<>
		
	};

}

#endif

