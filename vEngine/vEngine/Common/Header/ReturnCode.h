//
//  ReturnCode.hpp
//  AppFrame
//
//  Created by Yuan Li on 2015/09/20.
//  Copyright © 2015 Yuan Li. All rights reserved.
//

#ifndef ReturnCode_hpp
#define ReturnCode_hpp

#include <stdio.h>
#include <string>

namespace MocapGE {

    class ReturnCode
    {
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
    
    static ReturnCode Success(std::string Message = "Success")
    {
        return ReturnCode(0, Message);
    }
    
    static ReturnCode Failure(std::string Message = "Failure")
    {
        return ReturnCode(-1, Message);
    }
}

#endif /* ReturnCode_hpp */
