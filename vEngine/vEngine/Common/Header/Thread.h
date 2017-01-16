//
//  Thread.hpp
//  AppFrame
//
//  Created by Yuan Li on 2015/09/28.
//  Copyright © 2015 Yuan Li. All rights reserved.
//

#ifndef Thread_hpp
#define Thread_hpp

#include <string>
#include "Common\Header\ReturnCode.h"
#include <thread>


namespace MocapGE
{
    class Thread
    {
    public:
        Thread();
        virtual ~Thread();
        
        virtual ReturnCode Main(void* para) = 0;
        
        ReturnCode Create(void* Parameter_ = nullptr,std::string Name = __FILE__);
        
        ReturnCode Join();
        
    private:
        std::string Name;
        void* Parameter;        
        
        std::thread ThreadInstance;
        //pthread_attr_t ThreadAttr;

        
        static void* ThreadMain(void* ThreadData);
    };
}

#endif /* Thread_hpp */
