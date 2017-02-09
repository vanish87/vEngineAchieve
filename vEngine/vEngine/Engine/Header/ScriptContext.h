#ifndef SCRIPT_CONTEXT_H_
#define SCRIPT_CONTEXT_H_

#pragma once
#include "Common\Header\CommonPreDec.h"
#include "Common\Header\Thread.h"
#include <mutex>

namespace vEngine
{	
	//to monitor and reload script when it is necessary
	class ScriptThread : public Thread
	{
	public:
		ScriptThread();
		~ScriptThread();

		void SetPath(std::string PathToWatch);
	public:
		virtual ReturnCode Main(void* para) override;

	private:
		bool should_quit_;
		std::string current_path_;
		std::mutex mutex_;
	};

	typedef bool(*ScriptFunction)(void* UserData);
	struct ScriptFuctionDescription
	{
		std::string name_;
		uint32_t parameter_num_;
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

		void SetupBuildinFunctions();
		void StartMonitorPath(std::string PathToWatch);
	private:
		ScriptThread monitor_thread_;
		std::mutex mutex_;
	};
}

#endif