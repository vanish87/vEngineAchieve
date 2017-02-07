#include "Engine\Header\ScriptContext.h"
#include "Engine\Header\Lua\LuaScriptContext.h"
#include <filesystem>
#include <windows.h>


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
		DWORD dwWaitStatus;
		HANDLE dwChangeHandles[1];
		dwChangeHandles[0] = FindFirstChangeNotification(
			this->current_path_.c_str(),	// directory to watch 
			TRUE,							// do not watch subtree 
			FILE_NOTIFY_CHANGE_LAST_WRITE); // watch file name changes 

		if (dwChangeHandles[0] == INVALID_HANDLE_VALUE)
		{
			PRINT("ERROR: FindFirstChangeNotification function failed.");
			this->should_quit_ = true;
		}

		while (!this->should_quit_ && !this->current_path_.empty())
		{
			dwWaitStatus = WaitForMultipleObjects(1, dwChangeHandles, FALSE, INFINITE);

			switch (dwWaitStatus)
			{
			case WAIT_OBJECT_0:
				if (FindNextChangeNotification(dwChangeHandles[0]) == FALSE)
				{
					PRINT("\n ERROR: FindNextChangeNotification function failed.\n");
					this->should_quit_ = true;
				}
				break;
			case WAIT_TIMEOUT:

				// A timeout occurred, this would happen if some value other 
				// than INFINITE is used in the Wait call and no changes occur.
				// In a single-threaded environment you might not want an
				// INFINITE wait.

				PRINT("\nNo changes in the timeout period.\n");
				break;

			default:
				PRINT("\n ERROR: Unhandled dwWaitStatus.\n");
				this->should_quit_ = true;
				break;
			}

			std::this_thread::sleep_for(std::chrono::microseconds(10));
		}

		return RCSuccess();
	}

}
