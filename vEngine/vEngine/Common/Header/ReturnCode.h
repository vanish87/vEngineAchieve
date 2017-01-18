//
//  ReturnCode.hpp
//  AppFrame
//
//  Created by Yuan Li on 2015/09/20.
//  Copyright © 2015 Yuan Li. All rights reserved.
//

#ifndef ReturnCode_hpp
#define ReturnCode_hpp

#pragma once

#include <stdio.h>
#include <string>

namespace vEngine {

    class ReturnCode
    {
	public:
		enum CODE
		{
			SUCCEED = 0,
			FAILURE = -1,
		};
    public:
        ReturnCode(int Val, std::string Message_ = nullptr)
        {
            this->Code = Val;
            this->Message = Message_;
        }
        virtual ~ReturnCode(){};
        
        explicit ReturnCode(int &Val, std::string & Message_)
        {
            this->Code = Val;
            this->Message = Message_;
        }
        ReturnCode(const ReturnCode & rhs)//copy constructor
        {
            this->Code = rhs.Code;
            this->Message = rhs.Message;
        }
        
        ReturnCode& operator=(const ReturnCode & rhs)
        {
            if (this == &rhs) return *this;
            this->Code = rhs.Code;
            this->Message   = rhs.Message;
            return *this;
        }
        
        bool operator==(const ReturnCode& rhs) const
        {
            return this->Code == rhs.Code;
        }
        bool operator!=(const ReturnCode& rhs) const
        {
            return this->Code != rhs.Code;
        }
        
    private:
        ReturnCode();

        int Code;
        std::string Message;
    };
    
    static ReturnCode RCSuccess(std::string Message = "Success")
    {
        return ReturnCode(ReturnCode::SUCCEED, Message);
    }
    
    static ReturnCode RCFailure(std::string Message = "Failure")
    {
        return ReturnCode(ReturnCode::FAILURE, Message);
    }
}

#endif /* ReturnCode_hpp */
