#include "Engine/Header/ScriptContext.h"
#include "Engine/Header/ScriptStack.h"
#include "Engine/Header/Lua/LuaScriptContext.h"
#include "Engine/Header/Context.h"
#include <filesystem>
#include <windows.h>

#include "Engine/Header/Lua/LuaScriptFunctions.h"


namespace vEngine
{
	ScriptContext::ScriptContext()
	{

	}
	ScriptContext::~ScriptContext()
	{

	}

	bool ScriptContext::FunctionBegin(std::string FunctionName, ScriptStack* Stack)
	{
		for (ScriptFuctionDescriptionSharedPtr& i : this->script_functions_)
		{
			if (i->name_ == FunctionName)
			{
				ScriptFunctionCallType type = SFCT_Begin;
				Stack->SetAdditionalData(&type);
				i->fuction_(Stack);
			}
		}
		return true;
	}

	bool ScriptContext::FunctionEnd(std::string FunctionName, ScriptStack* Stack)
	{
		for (ScriptFuctionDescriptionSharedPtr& i : this->script_functions_)
		{
			if (i->name_ == FunctionName)
			{
				ScriptFunctionCallType type = SFCT_End;
				Stack->SetAdditionalData(&type);
				i->fuction_(Stack);
			}
		}
		return true;
	}

	bool ScriptContext::RegisterScriptFunction(const ScriptFuctionDescription& Description)
	{
		ScriptFuctionDescriptionSharedPtr NewCopy = std::make_shared<ScriptFuctionDescription>();
		NewCopy->name_ = Description.name_;
		NewCopy->type_ = Description.type_;
		NewCopy->parameter_num_ = Description.parameter_num_;
		NewCopy->fuction_ = Description.fuction_;
		this->script_functions_.push_back(NewCopy);
		return true;
	}

	ScriptContext* ScriptContext::CreateContext()
	{
		//TODO if there is new script implementation
		//choose correct one here
		return &LuaScriptContext::GetInstance();
	}

	void ScriptContext::SetupBuildinFunctions()
	{
		ScriptContext& context = Context::Instance().GetScriptContext();
		ScriptFuctionDescription desc;
		desc.name_ = "SetGbuffer1";
		desc.fuction_ = Lua_SetGbuffer1;
		context.RegisterCppFunction(desc);
		desc.name_ = "SetGbuffer2";
		desc.fuction_ = Lua_SetGbuffer2;
		context.RegisterCppFunction(desc);
		desc.name_ = "SetLighting";
		desc.fuction_ = Lua_SetLighting;
		context.RegisterCppFunction(desc);
		desc.name_ = "SetGbufferIndex";
		desc.parameter_num_ = 1;
		desc.fuction_ = Lua_SetGbufferIndex;
		context.RegisterCppFunction(desc);
		desc.name_ = "Update";
		desc.parameter_num_ = 0;
		desc.fuction_ = CPP_Update;
		context.RegisterScriptFunction(desc);
	}

	void ScriptContext::StartMonitorPath(std::string PathToWatch)
	{
		this->monitor_thread_.SetPath(PathToWatch);
		this->monitor_thread_.Create(nullptr);
	}

	void ScriptContext::Quit()
	{
		this->monitor_thread_.Quit();
	}

	ScriptClassDescriptionSharedPtr ScriptContext::MakeCopyFrom(const ScriptClassDescription& Description)
	{
		ScriptClassDescriptionSharedPtr NewClassCopy = std::make_shared<ScriptClassDescription>(Description.name_);
		for (const ScriptFuctionDescriptionSharedPtr& i : Description.fuctions_)
		{
			ScriptFuctionDescriptionSharedPtr NewCopy = std::make_shared<ScriptFuctionDescription>();
			NewCopy->name_ = i->name_;
			NewCopy->type_ = i->type_;
			NewCopy->parameter_num_ = i->parameter_num_;
			NewCopy->fuction_ = i->fuction_;
			NewClassCopy->fuctions_.push_back(NewCopy);
		}
		NewClassCopy->constructor_ = Description.constructor_;
		return NewClassCopy;
	}

	ScriptThread::ScriptThread()
	{

	}

	ScriptThread::~ScriptThread()
	{

	}

	void ScriptThread::SetPath(std::string PathToWatch)
	{
		if (!this->current_path_.empty()) PRINT_AND_BREAK("cannot change path while running at this time");

		std::experimental::filesystem::path path = PathToWatch;
		if (path.is_relative())
		{
			path = std::experimental::filesystem::absolute(path);
		}
		std::unique_lock<std::mutex> lk(mutex_);
		this->current_path_ = path.generic_string();
	}

	void ScriptThread::Quit()
	{
		this->should_quit_ = true;
		this->ThreadInstance.detach();
	}

	ReturnCode ScriptThread::Main(void* para)
	{
		if (this->current_path_.empty()) return RCFailure();

		//windows dependent code

		HANDLE hDir = CreateFile(std::wstring(current_path_.begin(), current_path_.end()).c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL);

		if (hDir == INVALID_HANDLE_VALUE) {
			PRINT_ERROR("CreateFile failed.");
			return RCFailure();
		}

		bool EventHanlded = false;

		while (!this->should_quit_ && !this->current_path_.empty())
		{
			wchar_t lpBuffer[1024] = { L'\0' };
			DWORD dwBytesReturned;
			DWORD dwNotifyFilter;
			BOOL bRet;

			dwNotifyFilter =
				FILE_NOTIFY_CHANGE_LAST_WRITE;

			bRet = ReadDirectoryChangesW(hDir,  
				lpBuffer, 
				sizeof(lpBuffer) / sizeof(lpBuffer[0]), 
				TRUE,
				dwNotifyFilter, 
				&dwBytesReturned, 
				NULL,
				NULL);

			if (!bRet) {
				PRINT_ERROR("ReadDirectoryChangesW failed.");
				break;
			}

			int i = 0;
			bool EventHanldedThisFrame = false;
			while (!EventHanlded) {
				FILE_NOTIFY_INFORMATION *lpInfomation
					= (FILE_NOTIFY_INFORMATION *)&lpBuffer[i];

				wchar_t filename[1024] = { L'\0' };
				const size_t length = sizeof(filename) / sizeof(filename[0]);
				lpInfomation->FileName[lpInfomation->FileNameLength / sizeof(wchar_t)] = L'\0';
				std::wstring FileName = std::wstring(current_path_.begin(), current_path_.end()) + std::wstring(lpInfomation->FileName);

				switch (lpInfomation->Action) {

				case FILE_ACTION_MODIFIED:
					PRINT("file modified: "<<std::string(FileName.begin(),FileName.end()));
					EventHanlded = true;
					EventHanldedThisFrame = true;
					break;

				case FILE_ACTION_ADDED:
				case FILE_ACTION_REMOVED:
				case FILE_ACTION_RENAMED_OLD_NAME:
				case FILE_ACTION_RENAMED_NEW_NAME:
				default:
					PRINT("Unknown File Action: " << FileName.c_str());
					break;
				}

				if (lpInfomation->NextEntryOffset == 0) {
					break;
				}
				i += lpInfomation->NextEntryOffset / 2;
			}

			if (!EventHanldedThisFrame)
			{
				EventHanlded = false;
			}

			this->Sleep(100);
		}

		return RCSuccess();
	}

}
