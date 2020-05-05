#include "Server.h"
#include "Connection.h"
#include "SystemSignalsHandler.h"

#include <variant>
#include <sstream>

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
    if (const auto error = _listener.init())
    {
        return error;
    }
    while(true)
    {
        fd_set readset;
        FD_ZERO(&readset);

        FD_SET(_listener.getRawSocket(), &readset);
        for(const auto& connection : _connections)
        {
            FD_SET(connection->getSocket().socket, &readset);
        }

        timeval timeout;
        timeout.tv_sec = 60;
        timeout.tv_usec = 0;

        if(select(getMaxSocketValue(), &readset, nullptr, nullptr, &timeout) < 0)
        {
            return Error(wrapErrorno("selecting was failed: "));
        }
    
        if (FD_ISSET(_listener.getRawSocket(), &readset))
        {
            handleListenerScoket();
        }

        for(auto connectionIt = _connections.begin(); connectionIt != _connections.end(); ++connectionIt)
        {
            if(FD_ISSET((*connectionIt)->getSocket().socket, &readset))
            {
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
    if (_connections.size() < _maxSimultaneousConnections)
    {
        //TODO: resolve std::visit
        auto result = _listener.acceptConnection();
        if(auto* socketHolder = std::get_if<SocketHolder>(&result))
        {
            _connections.emplace_back(std::make_unique<Connection>(std::move(*socketHolder)));
        }else if(const auto* error = std::get_if<Error>(&result))
        {
            Logger::error(*error);
        }
    }
}

void Server::handleClientConnection(const Connections::const_iterator& connIt)
{
    const auto& connection = *connIt;
    const auto& result = connection->read();
    //TODO: use std::visit
    if(const auto* dataPacket = std::get_if<DataPacket>(&result))
    {
        const auto commandLastPos = dataPacket->find_first_of(" ");
        std::string command;
        std::string arguments;
        if (commandLastPos != std::string::npos)
        {
            command = dataPacket->substr(0, commandLastPos);
            arguments = dataPacket->substr(commandLastPos+1, dataPacket->size() - 1);
        }
        else
        {
            command = *dataPacket;
        }
        
        const auto actionIt = _actions.find(command);
        if (actionIt != _actions.end()){
            if(const auto& error = connection->sendAck())
            {
                Logger::error(error);
            }
            else
            {
                actionIt->second(arguments, connIt);
            }
        }
        std::stringstream ss;
        ss << "command: " << command << "; arguments: "<< arguments;
        Logger::log(ss.str());
    }
    else if(const auto* error = std::get_if<Error>(&result))
    {
        Logger::error(*error);
    }else if(const auto* closeTag = std::get_if<CloseTag>(&result))
    {
        closeClientConnection(connIt);
    }
}

int Server::getMaxSocketValue() const
{
    auto maxSocketValue = _listener.getRawSocket();
    if(!_connections.empty())
    {
        const auto maxIt = std::max_element(_connections.cbegin(),
                                                          _connections.cend(),
                                                          [](const Connections::value_type& lhs,
                                                             const Connections::value_type& rhs)
                                                          {
                                                              return lhs->getSocket() < rhs->getSocket();
                                                          });
        
        
        maxSocketValue = std::max((*maxIt)->getSocket().socket, maxSocketValue);
    }
    return maxSocketValue + 1;
}

