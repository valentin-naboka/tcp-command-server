#pragma once
#include <string>

class Logger final
{
public:
    static void log(std::string str);
    static void error(std::string error);
};
