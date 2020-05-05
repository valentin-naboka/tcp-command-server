#pragma once

#include <string>

class Error
{
public:
    
    Error () = default;
    Error (std::string error);
    
    operator std::string() const;
    operator bool() const;
    
private:
    std::string _errorMsg;
};

std::string wrapErrorno(std::string description);
