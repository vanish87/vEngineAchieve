#include "Engine/Header/ScriptTest.h"

namespace vEngine
{
	ScriptTest::ScriptTest()
	{

	}

	void ScriptTest::PrintError(lua_State* state)
	{
		const char* ErrorMessage = lua_tostring(state, -1);
		PRINT_AND_BREAK(ErrorMessage);
		lua_pop(state, 1);
	}
	void ScriptTest::CheckAndPrintError(lua_State* state, int Result)
	{
		if (Result != LUA_OK)
		{
			PrintError(state);
		}
	}

	void ScriptTest::Run()
	{
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);
		int Result = luaL_loadfile(L,"LuaScript/HelloWorld.lua");
		CheckAndPrintError(L, Result);
		Result = lua_pcall(L, 0, 0, 0);
		CheckAndPrintError(L, Result);
		lua_close(L);
	}
}
