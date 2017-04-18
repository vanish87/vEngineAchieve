#include "Engine/Header/Profiler.h"

namespace vEngine
{
	Profiler::Profiler(const std::string ProfilerName)
		:Name(ProfilerName), Enabled(false)
	{
	}

	Profiler::~Profiler()
	{

	}

	void Profiler::Begin(PROFILER_EVENT EventType)
	{
		this->EventTimer.Retart();
	}

	void Profiler::End(PROFILER_EVENT EventType)
	{
		float Time = this->EventTimer.Timef();
		this->EventTimer.Retart();
		for (auto& it : this->HandlerList)
		{
			if (it->Process(EventType))
			{
				break;
			}
		}
	}

	void Profiler::RegisterEventHandler(ProfilerEventHandler* Handler)
	{
		for (auto& it : this->HandlerList)
		{
			if (it->Signature == Handler->Signature)
			{
				return;
			}
		}

		this->HandlerList.push_back(Handler);
	}

	void Profiler::UnRegisterEventHandler(ProfilerEventHandler* Handler)
	{
		for (auto it = this->HandlerList.begin(); it != this->HandlerList.end();  ++it)
		{
			if ((*it)->Signature == Handler->Signature)
			{
				this->HandlerList.erase(it);
				return;
			}
		}
	}

	void Profiler::SetEnable(bool Enable)
	{
		this->Enabled = Enable;
	}

}

