#include "Engine\Header\ScriptContext.h"
#include "Engine\Header\Lua\LuaScriptContext.h"


namespace vEngine
{
	ScriptContext::ScriptContext()
	{

	}
	ScriptContext::~ScriptContext()
	{

	}

	ScriptContext* ScriptContext::CreateContext()
	{
		//TODO if there is new script implementation
		//choose correct one here
		return &LuaScriptContext::GetInstance();
	}

}
