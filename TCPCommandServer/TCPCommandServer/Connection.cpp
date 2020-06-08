//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com.
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#include "Connection.h"

#include <algorithm>
#include <errno.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(SocketHolder socket)
    : _socketHolder(std::move(socket))
{
}

const SocketHolder& Connection::getSocket() const
{
    return _socketHolder;
}

ConnectionResult Connection::read() const
{
    DataPacket packet;
    const auto BufferLenght = 1024u;
    char buffer[BufferLenght] = { 0 };

    auto readSize = 0l;
    while ((readSize = recv(_socketHolder.socket, buffer, BufferLenght, 0)) > 0) {
        std::copy_if(buffer,
            buffer + readSize,
            std::back_inserter(packet),
            [](const char c) {
                return c != '\n' && c != '\r';
            });
    }

    if (readSize < 0 && (errno != EWOULDBLOCK || errno != EAGAIN)) {
        return std::string("receiving was failed: ") + std::strerror(errno);
    }

    if (readSize == 0) {
        return CloseTag {};
    }
    return packet;
}

Error Connection::write(const DataPacket& data)
{
    auto sentSize = 0;
    const auto dataSize = data.size();
    while (sentSize < dataSize) {
        const auto chunkSentSize = send(_socketHolder.socket, data.data() + sentSize, data.size() - sentSize, 0);
        if (chunkSentSize < 0 && (errno != EWOULDBLOCK || errno != EAGAIN || errno != EINTR)) {
            return std::string("sending was failed: ") + std::strerror(errno);
        }
        sentSize += chunkSentSize;
    };

    return Error();
}

Error Connection::sendAck()
{
    return write("OK\n\r");
}

Error Connection::sendError()
{
    return write("ERROR\n\r");
}
