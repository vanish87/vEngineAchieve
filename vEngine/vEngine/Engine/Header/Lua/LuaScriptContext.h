#ifndef LUASCRIPT_CONTEXT_H_
#define LUASCRIPT_CONTEXT_H_

#pragma once
#include <vector>
#include "Engine\Header\ScriptContext.h"

struct lua_State;

namespace vEngine
{	
	class LuaScriptContext : public ScriptContext
	{
		SINGLETON_PRAVATE(LuaScriptContext)

	public:
		static LuaScriptContext& GetInstance()
		{
			static LuaScriptContext Instance;
			return Instance;
		}

		virtual bool RunString(std::string ScriptToRun) override;
		virtual bool RunFile(std::string FileName) override;
		virtual bool RegisterCppFunction(const ScriptFuctionDescription& Description) override;

		void PrintError(lua_State* state, int Result);
		void CheckAndPrintError(lua_State* state, int Result);

	private:
		lua_State* L;
		std::vector<ScriptFuctionDescription*> functions_;
	};
}

#endif