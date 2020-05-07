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
    /**
     * MG
     *
     * Potential bug
     *
     * Client can sent commands of any size.
     * If the command exceeds BufferLength below, we will receive incomplete command during the first Connection::read() call.
     * The remainder of the first commands will be received during subsequent Connection::read() call.
     * And both Connection::read() calls would result into errors.
     * Which is obviously wrong :)
     *
     * I don't treat this as a major error within the context of this test task but just wanted to give some overview on complexity
     * of this kind of tasks :)
     *
     * One of the solutions to this would be passing command-handler-callback to Connection::read(), feeding read bytes into Connection's private buffer (f.e. stringstream),
     * and monitoring command terminators ('\r's) in incoming data which would trigger a call to command-handler callback.
     * And then trimming all the white-spaces after that '\r'.
     *
     * Valentin
     * recv is called in the loop until recv return value is greater than 0, and then appends all read bytes to the DataPacket packet inside the loop.
     * Thus, if the command size is greater than bufferLenght we get bufferLenght bytes after first recv call and then receive the rest of bytes subsequently.
     * Let's discuss this.
     */
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

    /**
     * MG
     *
     * It is tricky here:
     * - if readSize is 0 and errno is EWOULDBLOCK or EAGAIN than we should probably keep reading
     *
     * Valentin
     * The doc states: "When a stream socket peer has performed an orderly shutdown, the
     * return value will be 0 (the traditional "end-of-file" return)." So, I suppose we need to handle such a case as a closing connection.
     *  http://man7.org/linux/man-pages/man2/recv.2.html#RETURN_VALUE
     */
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
