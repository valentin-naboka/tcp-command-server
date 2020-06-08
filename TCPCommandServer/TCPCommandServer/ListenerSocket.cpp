//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com.
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#include "ListenerSocket.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

ListenerSocket::ListenerSocket(const uint16_t port, const uint16_t maxSimultaneousConnections)
    : _port(port)
    , _maxSimultaneousConnections(maxSimultaneousConnections)
{
}

Error ListenerSocket::init()
{
    _listener.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_listener.socket == -1) {
        return Error(wrapErrorno("сould not create socket: "));
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(_port);

    if (bind(_listener.socket, (struct sockaddr*)&server, sizeof(server)) < 0) {
        return Error(wrapErrorno("сould not bind socket: "));
    }

    int reuseAddr = 1;
    if (setsockopt(_listener.socket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) < 0) {
        return Error(wrapErrorno("сould not set socket SO_REUSEADDR: "));
    }
    const uint8_t QueueLenght = _maxSimultaneousConnections;
    if (listen(_listener.socket, QueueLenght) > 0) {
        return Error(wrapErrorno("сould not set socket to the listen mode: "));
    }
    return Error();
}

ListenerSocketResult ListenerSocket::acceptConnection() const
{
    sockaddr_in client;
    int size = sizeof(struct sockaddr_in);
    const auto clientSock = accept(_listener.socket, (struct sockaddr*)&client, (socklen_t*)&size);
    if (clientSock < 0) {
        return Error(wrapErrorno("сould not accept connection: "));
    }

    int flags = 0;
    if ((flags = fcntl(clientSock, F_GETFL, 0)) < 0) {
        return Error(wrapErrorno("could not get socket flags: "));
    }

    /**
     * MG
     *
     * Kudos
     *
     * This is absolutely correct.
     * But I'd like to know why you put it here :)
     *
     * Valentin
     * Just to turn on "async mode" by default when I further use recv/send functions.
     */
    if (fcntl(clientSock, F_SETFL, flags | O_NONBLOCK) < 0) {
        return Error(wrapErrorno("could not set socket flags: "));
    }

    /**
     * MG
     *
     * Requirements violation
     *
     * Current connections number must be checked against _maxSimultaneousConnections here
     * and all excess connections must be refused by closing client socket.
     *
     * Valentin
     * Please, see Server::handleListenerScoket(). The requirement implemented there. It seemed to me, that it is a good idea to give the possibility to the client to wait until the available quote to connection is released.
     * But a better idea is to implement it with a timeout.
     */
    return SocketHolder(clientSock);
}

int ListenerSocket::getRawSocket() const
{
    return _listener.socket;
}
