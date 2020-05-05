//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com. 
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#pragma once
#include <string>

class Logger final
{
public:
    static void log(std::string str);
    static void error(std::string error);
};
