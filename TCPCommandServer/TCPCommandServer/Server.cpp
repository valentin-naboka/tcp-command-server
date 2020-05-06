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
    /***
     * MG
     *
     * Nitpick
     *
     * Having signal handlers is ok but it is better to have them installed
     * from main() context.
     *
     * This would allow Server class to be reused in other applications/contexts.
     */
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

        /**
         * MG
         *
         * Minor in this case
         *
         * select() return value should be handled.
         * At least for error logging purposes.
         *
         */
        if (FD_ISSET(_listener.getRawSocket(), &readset)) {
            handleListenerScoket();
        }

        for (auto connectionIt = _connections.begin(); connectionIt != _connections.end(); ++connectionIt) {
            if (FD_ISSET((*connectionIt)->getSocket().socket, &readset)) {
                handleClientConnection(connectionIt);
            }
        }
    }
    return Error();
}

void Server::closeClientConnection(const Connections::const_iterator& it)
{
    _connections.erase(it);
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

void Server::handleClientConnection(const Connections::const_iterator& connIt)
{
    const auto& connection = *connIt;
    const auto& result = connection->read();
    if (const auto* dataPacket = std::get_if<DataPacket>(&result)) {
        const auto commandLastPos = dataPacket->find_first_of(" ");
        std::string command;
        std::string arguments;
        if (commandLastPos != std::string::npos) {
            command = dataPacket->substr(0, commandLastPos);
            /**
             * MG
             *
             * Minor requirement violation
             *
             * req: Command and its arguments are separated with space(s) ...
             *
             * Excess leading spaces must be trimmed out from argument here.
             *
             */
            arguments = dataPacket->substr(commandLastPos + 1, dataPacket->size() - 1);
        } else {
            command = *dataPacket;
        }

        const auto actionIt = _actions.find(command);
        if (actionIt != _actions.end()) {
            if (const auto& error = connection->sendAck()) {
                Logger::error(error);
            } else {
                /**
                 * MG
                 *
                 * Critical error (UB):
                 *
                 * Server::handleClientConnection() is called from loop over _connections
                 * actionIt->second() being 'exit' handler calls Server::closeClientConnection() which modifies (erases element from) _conections
                 *
                 */
                actionIt->second(arguments, connIt);
            }
        }
        std::stringstream ss;
        ss << "command: " << command << "; arguments: " << arguments;
        Logger::log(ss.str());
    } else if (const auto* error = std::get_if<Error>(&result)) {
        /**
         * MG
         *
         * Looks like an error.
         *
         * Not triggering connection close here might potentially lead to errored connections not being removed from _connections
         * and thus to resource leakage (both memory and file descriptors).
         *
         * In case of unrecoverable error in network communication it make sense to close the connection
         * since it is in undefined state for us anyway.
         * Better to define that state by closing it :)
         *
         */
        Logger::error(*error);
    } else if (const auto* closeTag = std::get_if<CloseTag>(&result)) {
        /**
         * MG
         *
         * Critical error (UB):
         *
         * Server::handleClientConnection() is called from loop over _connections
         * Server::closeClientConnection() modifies (erases element from) _connections
         *
         */
        closeClientConnection(connIt);
    }
}

int Server::getMaxSocketValue() const
{
    auto maxSocketValue = _listener.getRawSocket();
    /**
     * MG
     *
     * Nitpick
     *
     * A bit over-engineered.
     * The only reason could be if std::max_element had O(n) complexity.
     * But it is not :)
     *
     * The following would do the trick with less lines of code:
     *
     * for (const auto& c : _connections) {
     *	maxSocket_value = std::max(maxSocketValue, c.getSocket());
     * }
     *
     */
    if (!_connections.empty()) {
        const auto maxIt = std::max_element(_connections.cbegin(),
            _connections.cend(),
            [](const Connections::value_type& lhs,
                const Connections::value_type& rhs) {
                return lhs->getSocket() < rhs->getSocket();
            });

        /**
         * MG
         *
         * For empty list std::max_element() returns cend
         * and maxIt must be check against it here
         */
        maxSocketValue = std::max((*maxIt)->getSocket().socket, maxSocketValue);
    }
    return maxSocketValue + 1;
}
