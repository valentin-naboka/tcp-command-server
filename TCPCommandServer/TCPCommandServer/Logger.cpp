#include "Logger.h"
#include <iostream>

void Logger::log(std::string str)
{
    std::cout<<str<<std::endl;
}

void Logger::error(std::string error)
{
    std::cerr<<error;
}
