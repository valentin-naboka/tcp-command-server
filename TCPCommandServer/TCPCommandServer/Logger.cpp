//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com.
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#include "Logger.h"
#include <iostream>

void Logger::log(std::string str)
{
    std::cout << str << std::endl;
}

void Logger::error(std::string error)
{
    std::cerr << error << std::endl;
}
