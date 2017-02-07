#ifndef SCRIPT_CONTEXT_H_
#define SCRIPT_CONTEXT_H_

#pragma once
#include "Common\Header\CommonPreDec.h"

namespace vEngine
{	
	typedef bool(*ScriptFunction)(void* UserData);
	struct ScriptFuctionDescription
	{
		std::string name_;
		ScriptFunction fuction_;
	};
	class ScriptContext
	{
	public:
		ScriptContext();
		virtual ~ScriptContext();
	public:

		virtual bool RunString(std::string ScriptToRun) = 0;
		virtual bool RunFile(std::string FileName) = 0;

		virtual bool RegisterCppFunction(const ScriptFuctionDescription& Description) = 0;

		static ScriptContext* CreateContext();
	};
}

#endif