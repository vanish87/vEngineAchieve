#ifndef SCRIPT_CONTEXT_H_
#define SCRIPT_CONTEXT_H_

#pragma once
#include "Common\Header\CommonPreDec.h"
#include "Common\Header\Thread.h"
#include "Engine\Header\EnginePreDec.h"
#include <mutex>
#include <vector>

namespace vEngine
{	
	class ScriptStack;
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
		enum FunctionType
		{
			FT_CONSTRUCTOR,
			FT_DESTRUCTOR,
			FT_NORMAL,
		};
		std::string name_;
		FunctionType type_;
		uint32_t parameter_num_;

		ScriptFunction fuction_;

		ScriptFuctionDescription()
			:name_("INVALID"), type_(FT_NORMAL), parameter_num_(0), fuction_(nullptr)
		{

		}
	};

	class ScriptClassDescription
	{	
		friend class ScriptContext;
		friend class LuaScriptContext;
	private:
		std::string name_;
		std::vector<ScriptFuctionDescriptionSharedPtr> fuctions_;
		ScriptFuctionDescriptionSharedPtr constructor_;
		ScriptClassDescription() {};
	public:
		ScriptClassDescription(std::string name)
			:name_(name)
		{

		}

		void RegisterFunction(const ScriptFuctionDescriptionSharedPtr& FuncDesc)
		{
			switch (FuncDesc->type_)
			{
			case ScriptFuctionDescription::FT_CONSTRUCTOR:
				constructor_ = FuncDesc;
				break;
			case ScriptFuctionDescription::FT_DESTRUCTOR:
				break;
			case ScriptFuctionDescription::FT_NORMAL:
				fuctions_.push_back(FuncDesc);
				break;
			default:
				break;
			}
		};
				
	};

	class ScriptContext
	{
	public:
		ScriptContext();
		virtual ~ScriptContext();
	public:

		virtual bool RunString(std::string ScriptToRun) = 0;
		virtual bool RunFile(std::string FileName, std::string FunctionName = "") = 0;

		virtual bool RegisterCppFunction(const ScriptFuctionDescription& Description) = 0;
		virtual bool RegisterCppClass(const ScriptClassDescription& Description) = 0;

		virtual bool RegisterScriptFunction(const ScriptFuctionDescription& Description);

		static ScriptContext* CreateContext();

		void SetupBuildinFunctions();

		//this function should handle "FunctionName"'s input(aka. Parameters)
		//to push parameter here
		bool FunctionBegin(std::string FunctionName, ScriptStack* Stack);
		//this function should handle "FunctionName"'s output(aka. Return values)
		//to pop return value here
		bool FunctionEnd(std::string FunctionName, ScriptStack* Stack);

		void StartMonitorPath(std::string PathToWatch);
	private:
		ScriptThread monitor_thread_;
		std::mutex mutex_;
	protected:
		std::vector<ScriptFuctionDescriptionSharedPtr> cpp_functions_;
		std::vector<ScriptFuctionDescriptionSharedPtr> script_functions_;
		std::vector<ScriptClassDescriptionSharedPtr> cpp_classes_;

		ScriptClassDescriptionSharedPtr MakeCopyFrom(const ScriptClassDescription& Description);
	};
}

#endif