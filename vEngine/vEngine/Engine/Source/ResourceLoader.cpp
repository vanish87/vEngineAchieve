#include "Engine\Header\ResourceLoader.h"
#include "Engine\Header\GameObject.h"

#include "Engine\Header\Profiler.h"

namespace vEngine
{

	ResourceLoadingThread::ResourceLoadingThread()
	{

	}

	ResourceLoadingThread::~ResourceLoadingThread()
	{

	}

	ReturnCode ResourceLoadingThread::Main(void* para)
	{
		while (!should_quit_)
		{
			std::unique_lock<std::mutex> lk(mutex_);
			while (this->loading_queue_.empty())
			{
				cond_variable_.wait(lk);
				if (should_quit_) return RCSuccess();
			}
			ThreadJob* job = loading_queue_.front();
			loading_queue_.pop();
			lk.unlock();
			job->Run();
			delete job;
			this->Sleep(10);
		}

		return RCSuccess();
	}

	void ResourceLoadingThread::AddToQueue(ThreadJob* job)
	{
		std::unique_lock<std::mutex> lk(mutex_);
		this->loading_queue_.push(job);
		this->cond_variable_.notify_one();
	}

	void ResourceLoadingThread::Quit()
	{
		this->should_quit_ = true;
		this->cond_variable_.notify_one();
	}

	ResourceLoader::ResourceLoader()
	{
		this->loading_thread_.Create(nullptr);
	}
	ResourceLoader::~ResourceLoader()
	{
		this->loading_thread_.Quit();
		this->loading_thread_.Join();
	}

	void ResourceLoader::AddAsync(GameObject* ObjectToLoad, CompleteCallBack callback /*= nullptr*/)
	{
		this->loading_thread_.AddToQueue(new ResourceLoadingJob(ObjectToLoad, callback));
	}

	ResourceLoadingJob::ResourceLoadingJob(GameObject* JobObj, CompleteCallBack callback /*= nullptr*/)
	{
		this->object_to_load_ = JobObj;
		this->complete_call_back_ = callback;
	}

	ResourceLoadingJob::~ResourceLoadingJob()
	{

	}
	
	void ResourceLoadingJob::Run()
	{
		ProfileLogHandler ResPro("LoadProfile");
		Profiler ModelLoad("LoadProfiler");
		ModelLoad.SetEnable(true);
		ModelLoad.RegisterEventHandler(&ResPro);
		ModelLoad.Begin(Profiler::PE_FUNCTION_CALL);
		this->object_to_load_->Load();
		ModelLoad.End(Profiler::PE_FUNCTION_CALL, "ResourceLoadingJob::Load");
		if (this->complete_call_back_ != nullptr)
		{
			this->complete_call_back_(this->object_to_load_);
		}
	}


}
