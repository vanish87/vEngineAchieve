#include "Engine\Header\ScriptContext.h"
#include "Engine\Header\Lua\LuaScriptContext.h"
#include "Engine\Header\Context.h"
#include <filesystem>
#include <windows.h>

#include "Engine\Header\Lua\LuaScriptFunctions.h"


namespace vEngine
{
	ScriptContext::ScriptContext()
	{

	}
	ScriptContext::~ScriptContext()
	{

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
		desc.fuction_ = Lua_SetGbuffer2;
		context.RegisterCppFunction(desc);
	}

	void ScriptContext::StartMonitorPath(std::string PathToWatch)
	{
		this->monitor_thread_.SetPath(PathToWatch);
		this->monitor_thread_.Create(nullptr);
	}

	ScriptThread::ScriptThread()
	{

	}

	ScriptThread::~ScriptThread()
	{

	}

	void ScriptThread::SetPath(std::string PathToWatch)
	{
		if (!this->current_path_.empty()) PRINT_AND_ASSERT("cannot change path while running at this time");

		std::experimental::filesystem::path path = PathToWatch;
		if (path.is_relative())
		{
			path = std::experimental::filesystem::absolute(path);
		}
		std::unique_lock<std::mutex> lk(mutex_);
		this->current_path_ = path.generic_string();
	}

	ReturnCode ScriptThread::Main(void* para)
	{
		if (this->current_path_.empty()) return RCFailure();

		//windows dependent code

		HANDLE hDir = CreateFileW(std::wstring(current_path_.begin(), current_path_.end()).c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL);

		if (hDir == INVALID_HANDLE_VALUE) {
			PRINT("CreateFile failed.");
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
				PRINT("ReadDirectoryChangesW failed.");
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
					PRINT(std::string(FileName.begin(),FileName.end()));
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

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		return RCSuccess();
	}

}
