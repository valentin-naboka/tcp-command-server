//
//  Connection.cpp
//  MysqlProxy
//
//  Created by Valentin on 5/4/20.
//  Copyright © 2020 Valentin. All rights reserved.
//

#include "Connection.h"

#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <string>

Connection::Connection(SocketHolder socket)
: _socketHolder(std::move(socket))
{
}

const SocketHolder& Connection::getSocket() const
{
    return _socketHolder;
}

//TODO: check it
ConnectionResult Connection::read() const
{
    DataPacket packet;
    const auto BufferLenght = 1024u;
    char buffer[BufferLenght] = {0};
    
    auto readSize = 0l;
    while((readSize = recv(_socketHolder.socket, buffer, BufferLenght , 0)) > 0)
    {
        std::copy_if(buffer,
                     buffer + readSize,
                     std::back_inserter(packet),
                     [](const char c){
                                        return c != '\n' && c!= '\r';
                                    });
    }
    
    //TODO: resolve connection reset by peer
    if(readSize < 0 && (errno != EWOULDBLOCK || errno != EAGAIN))
    {
        return std::string("receiving was failed: ")  + strerror(errno);
    }
    return packet;
}

Error Connection::write(const DataPacket& data)
{
    auto sentSize = 0;
    const auto dataSize = data.size();
    while (sentSize < dataSize)
    {
        const auto chunkSentSize = send(_socketHolder.socket, data.data() + sentSize, data.size() - sentSize, 0);
        if (chunkSentSize == -1)
        {
            return Error(strerror(errno));
        }
        sentSize += chunkSentSize;
    };
    
    //TODO: resolve connection reset by peer
    if(sentSize < 0 && (errno != EWOULDBLOCK || errno != EAGAIN))
    {
        return std::string("sending was failed: ")  + strerror(errno);
    }
    return Error();
}

Connection::~Connection()
{
}
