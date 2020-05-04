#include "ListenerSocket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

//TODO: remove
using Error = std::logic_error;

ListenerSocket::ListenerSocket(const uint16_t port)
{
    _listener.socket = socket(AF_INET , SOCK_STREAM , 0);
    if (_listener.socket == -1)
    {
        throw Error("Could not create socket.");
    }
    
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    
    if( bind(_listener.socket,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        throw Error("Binding was failed.");
    }
    
    const uint8_t QueueLenght = 50;
    if(listen(_listener.socket , QueueLenght) > 0)
    {
        throw Error("Listening was failed.");
    }
}

SocketHolder ListenerSocket::acceptConnection() const
{
    sockaddr_in client;
    int size = sizeof(struct sockaddr_in);
    const auto clientSock = accept(_listener.socket, (struct sockaddr *)&client, (socklen_t*)&size);
    if (clientSock < 0)
    {
        throw Error("Accepting was failed.");
    }
    
    int flags = 0;
    if ((flags = fcntl(clientSock, F_GETFL, 0)) < 0)
    {
        throw Error("Couldn't get socket flags.");
    }
    
    if (fcntl(clientSock, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        throw Error("Couldn't set socket flags.");
    }
    
    return SocketHolder(clientSock);
}

int ListenerSocket::getRawSocket() const
{
    return _listener.socket;
}
