//
//  Error.h
//  MysqlProxy
//
//  Created by Valentin on 5/4/20.
//  Copyright © 2020 Valentin. All rights reserved.
//

#pragma once
#include <string>

class Error
{
public:
    Error () = default;
    
    Error (std::string error)
    : _errorMsg(std::move(error))
    {
    }
    
    operator std::string() const {
        return _errorMsg;
    }
private:
    std::string _errorMsg;
};

