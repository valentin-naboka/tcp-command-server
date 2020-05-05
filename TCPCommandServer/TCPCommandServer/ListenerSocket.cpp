//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com. 
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#include "ListenerSocket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

ListenerSocket::ListenerSocket(const uint16_t port, const uint16_t maxSimultaneousConnections)
    : _port(port)
    , _maxSimultaneousConnections(maxSimultaneousConnections)
{
}

Error ListenerSocket::init()
{
    _listener.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_listener.socket == -1)
    {
        return Error(wrapErrorno("сould not create socket: "));
    }
    
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(_port);
    
    if(bind(_listener.socket,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        return Error(wrapErrorno("сould not bind socket: "));
    }
    
    const uint8_t QueueLenght = _maxSimultaneousConnections;
    if(listen(_listener.socket , QueueLenght) > 0)
    {
        return Error(wrapErrorno("сould not set socket to the listen mode: "));
    }
    return Error();
}

ListenerSocketResult ListenerSocket::acceptConnection() const
{
    sockaddr_in client;
    int size = sizeof(struct sockaddr_in);
    const auto clientSock = accept(_listener.socket, (struct sockaddr *)&client, (socklen_t*)&size);
    if (clientSock < 0)
    {
        return Error(wrapErrorno("сould not accept connection: "));
    }
    
    int flags = 0;
    if ((flags = fcntl(clientSock, F_GETFL, 0)) < 0)
    {
        return Error(wrapErrorno("could not get socket flags: "));
    }
    
    if (fcntl(clientSock, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        return Error(wrapErrorno("could not set socket flags: "));
    }
    
    return SocketHolder(clientSock);
}

int ListenerSocket::getRawSocket() const
{
    return _listener.socket;
}
