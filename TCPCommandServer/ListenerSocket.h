#pragma once
#include "SocketHolder.h"
#include "Error.h"

#include <variant>

using ListenerSocketResult = std::variant<SocketHolder, Error>;

class ListenerSocket final
{
public:
    ListenerSocket(const uint16_t port, const uint16_t maxSimultaneousConnections);
    
    Error init();
    
    ListenerSocketResult acceptConnection() const;
    int getRawSocket() const;
    
private:
    SocketHolder _listener;
    const uint16_t _port;
    const uint16_t _maxSimultaneousConnections;
};
