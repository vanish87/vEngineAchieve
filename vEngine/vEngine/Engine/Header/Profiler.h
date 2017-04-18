#ifndef PROFILER_H_
#define PROFILER_H_

#pragma once
#include "Common\Header\Timer.h"
#include "Common\Header\StringHash.h"
#include "Engine\Header\EnginePreDec.h"
#include <vector>

namespace vEngine
{	
	class Profiler
	{
	public:
		enum PROFILER_EVENT
		{
			PE_FUNCTION_CALL
		};

		Profiler(const std::string ProfilerName);
		virtual ~Profiler();

		virtual void Begin(PROFILER_EVENT EventType);
		virtual void End(PROFILER_EVENT EventType);

		void RegisterEventHandler(ProfilerEventHandler* Handler);
		void UnRegisterEventHandler(ProfilerEventHandler* Handler);

		void SetEnable(bool Enable);

		void Dump();

	protected:
		std::vector<ProfilerEventHandler*> HandlerList;

		Timer EventTimer;
		std::string Name;
		bool Enabled;

	private:
		Profiler() {};
	};

	class ProfilerEventHandler
	{
		friend class Profiler;
	public:
		ProfilerEventHandler(string_hash Signature_)
			:Signature(Signature_)
		{

		};

		virtual ~ProfilerEventHandler() {};

		virtual bool Process(Profiler::PROFILER_EVENT Event) = 0;

	private:
		ProfilerEventHandler() {};

		string_hash Signature;
	};
}

#endif