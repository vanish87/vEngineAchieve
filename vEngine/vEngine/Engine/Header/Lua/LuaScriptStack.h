#ifndef LUA_SCRIPT_STACK_H_
#define LUA_SCRIPT_STACK_H_

#pragma once
#include "Engine\Header\ScriptStack.h"

namespace vEngine
{	
	class LuaScriptStack : public ScriptStack
	{
	public:
		LuaScriptStack();
		virtual ~LuaScriptStack();
	public:

		virtual bool PushInt(int value) override;
		virtual bool PopInt(int& value) override;
	};
}

#endif