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
    
    operator bool() const {
        return !_errorMsg.empty();
    }
    
private:
    std::string _errorMsg;
};

