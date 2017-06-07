#include "Engine/Header/Profiler.h"

#include <fstream>
#include <mutex>
#include "Common\Header\CommonPreDec.h"

#include "Engine\Header\Text.h"

namespace vEngine
{	
	static std::mutex LOG_MUTEX;

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

	void Profiler::End(PROFILER_EVENT EventType, std::string Name)
	{
		float Time = this->EventTimer.Timef();
		this->EventTimer.Retart();
		for (auto& it : this->HandlerList)
		{
			if (it->Process(EventType, Time, Name))
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

	ProfileLogHandler::ProfileLogHandler(std::string LogFileName)
		:ProfilerEventHandler(string_hash("ProfileLogHandler"))
	{
		if (LogFileName.empty())
		{
			PRINT_AND_BREAK("Empty logfile name");
		}
		LogFilePath = "ProfilerLogs\\" + LogFileName + ".csv";
		std::experimental::filesystem::path path = LogFilePath;
		if (path.is_relative())
		{
			path = std::experimental::filesystem::absolute(path);
		}
		if (!std::experimental::filesystem::is_directory(path))
		{
			std::experimental::filesystem::create_directories(path.parent_path());
		}
	}

	bool ProfileLogHandler::Process(Profiler::PROFILER_EVENT Event, float Time, std::string Name)
	{
		std::ofstream LogFile;
		std::unique_lock<std::mutex> lk(LOG_MUTEX);
		LogFile.open(this->LogFilePath, std::fstream::out | std::fstream::app);
		if (LogFile.is_open())
		{
			switch (Event)
			{
			case vEngine::Profiler::PE_FUNCTION_CALL:
				LogFile << "Function: " << Name << ',' << Time << " ms" <<std::endl;
				break;
			default:
				break;
			}
			LogFile.close();
		}
		return true;
	}	

	ProfileStatsHandler::ProfileStatsHandler()
		:ProfilerEventHandler(string_hash("ProfileStatsHandler"))
		, Counter(0)
	{

	}

	bool ProfileStatsHandler::Process(Profiler::PROFILER_EVENT Event, float Time, std::string Name)
	{
		switch (Event)
		{
		case vEngine::Profiler::PE_FUNCTION_CALL:
			if (this->EventStats.find(Name) == this->EventStats.end())
			{
				std::get<0>(this->EventStats[Name]) = Time;
				std::get<1>(this->EventStats[Name]) = 0;
			}
			else
			{
				std::get<0>(this->EventStats[Name]) += Time;
				std::get<1>(this->EventStats[Name]) ++;
			}
			break;
		default:
			break;
		}


		this->Counter++;
		//if (this->Counter % 300 == 0)
		{
			int InitPos = 0;
			for (auto& it: this->EventStats)
			{
				//PRINT("Current average " << it.first << " :" << std::get<0>(it.second) / std::get<1>(it.second));
				Text ProfileInfo = (L"Current average " + std::wstring(it.first.begin(), it.first.end()) + L" :" + std::to_wstring(std::get<0>(it.second) / std::get<1>(it.second)));
				ProfileInfo.SetRect(int4(20, InitPos+=20, 0, 0));
				ProfileInfo.Draw();
			}
		}

		return true;
	}

}

