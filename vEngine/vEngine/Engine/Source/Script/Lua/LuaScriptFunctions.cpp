#include "Engine\Header\Lua\LuaScriptFunctions.h"
#include "Engine\Header\ScriptStack.h"
#include "Engine\Header\Context.h"

namespace vEngine
{
	bool Lua_SetGbuffer1(void* UserData)
	{
		Context::Instance().GetRenderFactory().GetRenderEngine().GetDeferredRendering()->ToggleGbuffer(0);
		return true;
	}

	bool Lua_SetGbuffer2(void* UserData)
	{
		Context::Instance().GetRenderFactory().GetRenderEngine().GetDeferredRendering()->ToggleGbuffer(1);
		return true;
	}

	bool Lua_SetGbufferIndex(void* UserData)
	{
		ScriptStack* stack = static_cast<ScriptStack*>(UserData);
		int Index = -1;
		if(!stack->PopInt(Index)) return false;
		Context::Instance().GetRenderFactory().GetRenderEngine().GetDeferredRendering()->ToggleGbuffer(Index);
		return true;
	}
	
	bool Lua_SetLighting(void* UserData)
	{
		Context::Instance().GetRenderFactory().GetRenderEngine().GetDeferredRendering()->ToggleLighting();
		return true;
	}
}
