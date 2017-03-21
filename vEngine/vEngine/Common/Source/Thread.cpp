//
//  Thread.cpp
//  AppFrame
//
//  Created by Yuan Li on 2015/09/28.
//  Copyright © 2015 Yuan Li. All rights reserved.
//

#include "Common\Header\Thread.h"

namespace vEngine
{
    Thread::Thread()
    {
        
    };
    
    Thread::~Thread()
    {

    };
        
    ReturnCode Thread::Create(void* Parameter_,std::string Name_)
    {
        this->Name = Name_;
        this->Parameter = Parameter_;
        
        //pthread_attr_init(&this->ThreadAttr);
        //pthread_attr_setdetachstate(&this->ThreadAttr, PTHREAD_CREATE_JOINABLE);
                
		//pthread create here        
		//int rc = pthread_create(&this->ThreadNum, &this->ThreadAttr, ThreadMain, this);
		this->ThreadInstance = std::thread(ThreadMain, this);
        
        //pthread_attr_destroy(&this->ThreadAttr);
        //if (rc)
        {
        //    return Failure("pthread_create Error:" + std::to_string(rc));
        }
        
        
        return RCSuccess();
    };
    
    ReturnCode Thread::Join()
    {
		this->ThreadInstance.join();
        return RCSuccess();
    }

	void Thread::Sleep(uint32_t TimeInMicroSeconds)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(TimeInMicroSeconds));
	}
        
    void* Thread::ThreadMain(void* ThreadData)
    {
        //call pure virtual main here
        Thread* RunningInstance = static_cast<Thread*>(ThreadData);
        ReturnCode RC=RunningInstance->Main(RunningInstance->Parameter);
        if (RC != RCSuccess())
        {
            PRINT("Thread: "<< RunningInstance->ThreadInstance.get_id() << "returned Error\n");
        }
		//RunningInstance->ThreadInstance.detach();
		//return value is ignored by std::thread
		return ThreadData;
    };



}