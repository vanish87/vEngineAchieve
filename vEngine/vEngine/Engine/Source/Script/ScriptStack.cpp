#include "Engine\Header\ScriptStack.h"


namespace vEngine
{
	ScriptStack::ScriptStack(ScriptContext* context)
		:context_(context)
	{

	}
	ScriptStack::~ScriptStack()
	{

	}

	uint32_t ScriptStack::PushedCount()
	{
		return this->pushed_count_;
	}

}
