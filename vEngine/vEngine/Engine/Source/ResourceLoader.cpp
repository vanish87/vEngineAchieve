#include "Engine\Header\ResourceLoader.h"
#include "Engine\Header\GameObject.h"

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
			}
			ThreadJob* job = loading_queue_.front();
			loading_queue_.pop();
			lk.unlock();
			job->Run();
			delete job;
			std::this_thread::sleep_for(std::chrono::microseconds(10));
		}

		return RCSuccess();
	}

	void ResourceLoadingThread::AddToQueue(ThreadJob* job)
	{
		std::unique_lock<std::mutex> lk(mutex_);
		this->loading_queue_.push(job);
		this->cond_variable_.notify_one();
	}

	ResourceLoader::ResourceLoader()
	{
		this->loading_thread_.Create(nullptr);
	}
	ResourceLoader::~ResourceLoader()
	{
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
		this->object_to_load_->Load();
		if (this->complete_call_back_ != nullptr)
		{
			this->complete_call_back_(this->object_to_load_);
		}
	}

}
