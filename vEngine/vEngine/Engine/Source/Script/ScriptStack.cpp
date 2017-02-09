#include "Engine\Header\ScriptStack.h"


namespace vEngine
{
	ScriptStack::ScriptStack(ScriptContext* context)
		:context_(context),pushed_count_(0),additional_data_(nullptr)
	{

	}
	ScriptStack::~ScriptStack()
	{

	}

	void ScriptStack::SetAdditionalData(void* data)
	{
		this->additional_data_ = data;
	}

	void* ScriptStack::GetAdditionalData()
	{
		return this->additional_data_;
	}

	uint32_t ScriptStack::PushedCount()
	{
		return this->pushed_count_;
	}

}
