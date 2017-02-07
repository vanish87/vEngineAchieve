#ifndef SCRIPT_TEST_H_
#define SCRIPT_TEST_H_

#pragma once

#ifdef __cplusplus
#include "lua\lua.hpp"
#else
extern "C"
{
	#include "lua\lapi.h"
	#include "lua\lauxlib.h"
	#include "lua\lua.h"
	#include "lua\lualib.h"
}
#endif

#include "Common\Header\CommonPreDec.h"

namespace vEngine
{
	
	class ScriptTest
	{
		SINGLETON_PRAVATE(ScriptTest);

	public:
		static ScriptTest& GetInstance()
		{
			static ScriptTest Instance;
			return Instance;
		}

		static void PrintError(lua_State* state);
		static void CheckAndPrintError(lua_State* state, int Result);
		void Run();
	};
}

#endif