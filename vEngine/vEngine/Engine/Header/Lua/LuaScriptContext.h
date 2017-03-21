#ifndef LUASCRIPT_CONTEXT_H_
#define LUASCRIPT_CONTEXT_H_

#pragma once
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
		virtual bool RunFile(std::string FileName, std::string FunctionName = "") override;
		virtual bool RegisterCppFunction(const ScriptFuctionDescription& Description) override;
		virtual bool RegisterCppClass(const ScriptClassDescription& Description) override;

		void PrintError(lua_State* state, int Result);
		void CheckAndPrintError(lua_State* state, int Result);

		lua_State* state();


	private:
		lua_State* L;
	};
}

#endif