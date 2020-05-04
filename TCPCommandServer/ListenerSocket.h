#pragma once
#include "SocketHolder.h"

class ListenerSocket final
{
public:
    ListenerSocket(const uint16_t port);
    
    SocketHolder acceptConnection() const;
    int getRawSocket() const;
    
private:
    SocketHolder _listener;
};
