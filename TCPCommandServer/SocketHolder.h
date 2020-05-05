#pragma once
#include "Logger.h"

#include <unistd.h>
#include <errno.h>
#include <string>

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
        if(socket != -1 && close(socket) != 0)
        {
            Logger::error(strerror(errno));
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
