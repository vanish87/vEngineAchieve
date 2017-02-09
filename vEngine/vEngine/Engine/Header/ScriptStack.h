#ifndef SCRIPT_STACK_H_
#define SCRIPT_STACK_H_

#pragma once
#include "Common\Header\CommonPreDec.h"

namespace vEngine
{	
	class ScriptContext;
	class ScriptStack
	{
	public:
		ScriptStack(ScriptContext* context);
		virtual ~ScriptStack();
	public:

		virtual bool PushInt(int value) = 0;
		virtual bool PopInt(int& value) = 0;

		uint32_t PushedCount();

	protected:
		uint32_t pushed_count_;
		ScriptContext* context_;
	};
}

#endif