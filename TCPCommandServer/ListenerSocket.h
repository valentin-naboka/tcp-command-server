#pragma once
#include "SocketHolder.h"

class ListenerSocket final
{
public:
    ListenerSocket(const uint16_t port, const uint16_t maxSimultaneousConnections);
    
    SocketHolder acceptConnection() const;
    int getRawSocket() const;
    
private:
    SocketHolder _listener;
    const uint16_t _maxSimultaneousConnections;
};
