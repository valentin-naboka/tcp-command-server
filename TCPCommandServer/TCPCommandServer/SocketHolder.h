//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com. 
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#pragma once
#include "Logger.h"

#include <unistd.h>
#include <errno.h>
#include <string>
#include <sstream>
#include <cstring>

struct SocketHolder final
{
    SocketHolder() = default;
    SocketHolder(const int socket)
        : socket(socket)
    {
    }
    
    SocketHolder(const SocketHolder&) = delete;
    const SocketHolder& operator=(const SocketHolder&) = delete;
    
    SocketHolder(SocketHolder&& other)
    : socket(other.socket)
    {
        other.socket = - 1;
    }

    SocketHolder& operator=(SocketHolder&& other)
    {
        socket = other.socket;
        other.socket = - 1;
        return *this;
    }
    
    ~SocketHolder()
    {
        if(socket != -1)
        {
            if(close(socket) != 0)
            {
                Logger::error(std::strerror(errno));
            }
            else
            {
                std::stringstream ss;
                ss<<"socket "<<socket<<" connection has been closed.";
                Logger::log(ss.str());
            }
        }
    }
    
    int socket = -1;
};

inline bool operator<(const SocketHolder& lhs, const SocketHolder& rhs)
{
    return lhs.socket < rhs.socket;
}

inline bool operator==(const SocketHolder& lhs, const SocketHolder& rhs)
{
    return lhs.socket == rhs.socket;
}
