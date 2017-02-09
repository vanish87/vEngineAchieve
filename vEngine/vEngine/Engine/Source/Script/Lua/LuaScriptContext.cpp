#include "Engine\Header\Lua\LuaScriptContext.h"
#include "Engine\Header\Lua\LuaScriptStack.h"

#ifdef __cplusplus
#include "lua\lua.hpp"
#else
COMPILE_PRINT_AND_ASSERT(false, "Should compile with c++")
#endif

namespace vEngine
{
	static int LuaFunctionInstance(lua_State* L)
	{
		CHECK_AND_ASSERT(lua_isuserdata(L, lua_upvalueindex(1)), "Check upvalues");
		ScriptFuctionDescription* func = static_cast<ScriptFuctionDescription*>(lua_touserdata(L, lua_upvalueindex(1)));
		//init a stack and set it as a parameter
		LuaScriptStack Stack;
		func->fuction_(&Stack);
		//return number of pushed result in this function
		return Stack.PushedCount();
	}

	LuaScriptContext::LuaScriptContext()
	{
		this->L = luaL_newstate();
		CHECK_AND_ASSERT(L != nullptr, "Lua init error");
		luaL_openlibs(L);
	}
	LuaScriptContext::~LuaScriptContext()
	{
		lua_close(L);
	}

	bool LuaScriptContext::RunString(std::string ScriptToRun)
	{
		int Result = luaL_loadstring(L, ScriptToRun.c_str());
		CheckAndPrintError(L, Result);
		Result = lua_pcall(L, 0, 0, 0);
		CheckAndPrintError(L, Result);
		return Result == LUA_OK;
	}

	bool LuaScriptContext::RunFile(std::string FileName, std::string FunctionName /*= ""*/)
	{
		int Result = luaL_loadfile(L, FileName.c_str());
		CheckAndPrintError(L, Result);
		bool IsFunctionCall = !FunctionName.empty();
		LuaScriptStack Stack;
		if (IsFunctionCall)
		{
			Result = lua_pcall(L, 0, 0, 0);
			CheckAndPrintError(L, Result);
			lua_getglobal(L, FunctionName.c_str());
			if (!lua_isfunction(L, -1))
			{
				lua_pop(L, 1);
				return false;
			}
			this->FunctionBegin(FunctionName, &Stack);
		}
		Result = lua_pcall(L, Stack.PushedCount(), LUA_MULTRET, 0);
		CheckAndPrintError(L, Result);
		if (IsFunctionCall)
		{
			//to get return value for RunFile caller
			//best way is to pack them to a return value container
			this->FunctionEnd(FunctionName, &Stack);
		}
		return Result == LUA_OK;
	}


	void LuaScriptContext::PrintError(lua_State* state, int Result)
	{
		const char* ErrorMessage = lua_tostring(state, -1);
		switch (Result)
		{
		case LUA_ERRRUN:
		{
			PRINT_AND_ASSERT("EXECUTION ERROR: " << ErrorMessage);
		}
		break;
		case LUA_ERRSYNTAX:
		{
			PRINT_AND_ASSERT("SYNTAX ERROR: " << ErrorMessage);
		}
		break;
		case LUA_ERRMEM:
		{
			PRINT_AND_ASSERT("MEMORY ERROR: " << ErrorMessage);
		}
		break;
		case LUA_ERRERR:
		{
			PRINT_AND_ASSERT("ERROR in ERROR HANDLER: " << ErrorMessage);
		}
		break;
		default:
			PRINT_AND_ASSERT(ErrorMessage);
			break;
		}
		lua_pop(state, 1);
	}
	void LuaScriptContext::CheckAndPrintError(lua_State* state, int Result)
	{
		if (Result != LUA_OK)
		{
			PrintError(state, Result);
		}
	}

	lua_State* LuaScriptContext::state()
	{
		return this->L;
	}

	bool LuaScriptContext::RegisterCppFunction(const ScriptFuctionDescription& Description)
	{
		ScriptFuctionDescription* NewCopy = new ScriptFuctionDescription();
		NewCopy->name_ = Description.name_;
		NewCopy->parameter_num_ = Description.parameter_num_;
		NewCopy->fuction_ = Description.fuction_;
		this->cpp_functions_.push_back(NewCopy);

		lua_pushlightuserdata(this->L, NewCopy);
		lua_pushcclosure(this->L, LuaFunctionInstance, 1 /*number of pushed data*/);
		lua_setglobal(L, NewCopy->name_.c_str());
		return true;
	}

}
