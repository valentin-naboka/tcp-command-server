//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com.
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#include "Server.h"
#include "Connection.h"
#include "SystemSignalsHandler.h"

#include <signal.h>
#include <sstream>
#include <variant>

Server::Server(const uint16_t port, const uint16_t maxSimultaneousConnections)
    : _listener(port, maxSimultaneousConnections)
    , _maxSimultaneousConnections(maxSimultaneousConnections)
{
    SignalHandler::setupConnections(_connections);

    SignalHandler::setupSignal(SIGINT);
    SignalHandler::setupSignal(SIGTERM);
    SignalHandler::setupSignal(SIGKILL);
}

void Server::registerAction(std::string name, Action action)
{
    _actions.emplace(std::make_pair(std::move(name), std::move(action)));
}

Error Server::run()
{
    if (const auto error = _listener.init()) {
        return error;
    }
    while (true) {
        fd_set readset;
        FD_ZERO(&readset);

        FD_SET(_listener.getRawSocket(), &readset);
        for (const auto& connection : _connections) {
            FD_SET(connection->getSocket().socket, &readset);
        }

        timeval timeout;
        timeout.tv_sec = 60;
        timeout.tv_usec = 0;

        if (select(getMaxSocketValue(), &readset, nullptr, nullptr, &timeout) < 0) {
            return Error(wrapErrorno("selecting was failed: "));
        }

        if (FD_ISSET(_listener.getRawSocket(), &readset)) {
            handleListenerScoket();
        }

        for (auto connectionIt = _connections.cbegin(); connectionIt != _connections.cend();) {
            if (FD_ISSET((*connectionIt)->getSocket().socket, &readset)) {
                connectionIt = handleClientConnection(std::move(connectionIt));
            } else {
                ++connectionIt;
            }
        }
    }
    return Error();
}

Connections::const_iterator Server::closeClientConnection(const Connections::const_iterator& it)
{
    return _connections.erase(it);
}

void Server::handleListenerScoket()
{
    if (_connections.size() < _maxSimultaneousConnections) {
        auto result = _listener.acceptConnection();
        if (auto* socketHolder = std::get_if<SocketHolder>(&result)) {
            _connections.emplace_back(std::make_unique<Connection>(std::move(*socketHolder)));
        } else if (const auto* error = std::get_if<Error>(&result)) {
            Logger::error(*error);
        }
    }
}

Connections::const_iterator Server::handleClientConnection(Connections::const_iterator connIt)
{
    const auto& connection = *connIt;
    const auto& result = connection->read();
    if (const auto* dataPacket = std::get_if<DataPacket>(&result)) {
        const auto commandLastPos = dataPacket->find_first_of(" ");
        std::string command;
        std::string arguments;
        if (commandLastPos != std::string::npos) {
            command = dataPacket->substr(0, commandLastPos);
            arguments = dataPacket->substr(commandLastPos + 1, dataPacket->size() - 1);
        } else {
            command = *dataPacket;
        }

        const auto actionIt = _actions.find(command);
        if (actionIt != _actions.end()) {
            if (const auto& error = connection->sendAck()) {
                Logger::error(error);
            } else {
                std::stringstream ss;
                ss << "command: " << command << "; arguments: " << arguments;
                Logger::log(ss.str());
                return actionIt->second(arguments, connIt);
            }
        }
        return std::move(++connIt);
    } else if (const auto* error = std::get_if<Error>(&result)) {
        Logger::error(*error);
        return closeClientConnection(connIt);
    } else if (const auto* closeTag = std::get_if<CloseTag>(&result)) {
        return closeClientConnection(connIt);
    }
    assert("None of the std::variant cases have been handled.");
    return _connections.cend();
}

int Server::getMaxSocketValue() const
{
    auto maxSocketValue = _listener.getRawSocket();
    for (const auto& c : _connections) {
        maxSocketValue = std::max(maxSocketValue, c->getSocket().socket);
    }

    return maxSocketValue + 1;
}
