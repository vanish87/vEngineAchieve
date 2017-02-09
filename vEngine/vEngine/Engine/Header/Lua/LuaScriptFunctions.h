#ifndef LUASCRIPT_FUNCTIONS_H_
#define LUASCRIPT_FUNCTIONS_H_

#pragma once

namespace vEngine
{
	bool Lua_SetGbuffer1(void* UserData);
	bool Lua_SetGbuffer2(void* UserData);
	bool Lua_SetLighting(void* UserData);
	bool Lua_SetGbufferIndex(void* UserData);


	bool CPP_Update(void* UserData);
}

#endif