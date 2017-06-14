#include "Engine/Header/Lua/LuaScriptStack.h"
#include "Engine/Header/Lua/LuaScriptContext.h"
#include "Engine/Header/Context.h"

#ifdef __cplusplus
#include "lua/lua.hpp"
#else
COMPILE_PRINT_AND_ASSERT(false, "Should compile with c++")
#endif

namespace vEngine
{
	LuaScriptStack::LuaScriptStack()
		//TODO investigate runtime context or register time context
		//this is runtime context
		:ScriptStack(&Context::Instance().GetScriptContext())
	{

	}
	LuaScriptStack::~LuaScriptStack()
	{

	}

	bool LuaScriptStack::PushInt(int value)
	{
		LuaScriptContext* context = static_cast<LuaScriptContext*>(this->context_);
		lua_pushinteger(context->state(), value);
		this->pushed_count_++;
		return true;
	}

	bool LuaScriptStack::PopInt(int& value)
	{
		LuaScriptContext* context = static_cast<LuaScriptContext*>(this->context_);
		if (!lua_isnumber(context->state(), -1)) return false;
		value = (int)lua_tointeger(context->state(), -1);
		lua_pop(context->state(), 1);
		this->pushed_count_--;
		return true;
	}

}
