#ifndef SCRIPT_STACK_H_
#define SCRIPT_STACK_H_

#pragma once
#include "Common\Header\CommonPreDec.h"

namespace vEngine
{	
	class ScriptContext;

	enum ScriptFunctionCallType
	{
		SFCT_Begin,
		SFCT_End,
	};

	class ScriptStack
	{
	public:
		ScriptStack(ScriptContext* context);
		virtual ~ScriptStack();
	public:

		virtual bool PushInt(int value) = 0;
		virtual bool PopInt(int& value) = 0;

		void  SetAdditionalData(void* data);
		void* GetAdditionalData();

		uint32_t PushedCount();

	protected:
		ScriptContext* context_;
		uint32_t pushed_count_;

		//optional user data for cpp functions
		//for script functions, it always indicates function call type:
		//which is ScriptFunctionCallType
		void* additional_data_;
	};
}

#endif