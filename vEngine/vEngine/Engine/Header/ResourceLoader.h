#ifndef RESOURCE_LOADER_H_
#define RESOURCE_LOADER_H_

#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Common\Header\CommonPreDec.h"
#include "Engine\Header\EnginePreDec.h"
#include "Common\Header\Thread.h"

#include "Engine\Header\TypeAndFormat.h"


namespace vEngine
{
	class ResourceLoadingThread : public Thread
	{
	public:
		ResourceLoadingThread();
		~ResourceLoadingThread();

	public:
		virtual ReturnCode Main(void* para) override;

		void AddToQueue(ThreadJob* job);
		void Quit();

	private:
		std::queue<ThreadJob*> loading_queue_;
		std::mutex mutex_;
		std::condition_variable cond_variable_;

		bool should_quit_;

	};
	class ResourceLoadingJob : public ThreadJob
	{
	public:
		ResourceLoadingJob(GameObject* JobObj, CompleteCallBack callback = nullptr);
		~ResourceLoadingJob();
	public:
		virtual void Run() override;

	private:
		ResourceLoadingJob() {};
		GameObject* object_to_load_;
		CompleteCallBack complete_call_back_;
	};
	class ResourceLoader
	{
		SINGLETON_PRAVATE(ResourceLoader)

	public:
		static ResourceLoader& GetInstance()
		{
			static ResourceLoader Instance;
			return Instance;
		};


		void AddAsync(GameObject* ObjectToLoad, CompleteCallBack callback = nullptr);
		//void AddSync();
	private:
		ResourceLoadingThread loading_thread_;

	};

}

#endif

