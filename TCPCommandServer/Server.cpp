#include "Server.h"
#include "Connection.h"
#include "SystemSignalsHandler.h"

#include <variant>
//TODO: remove
#include <iostream>

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
                    //TODO: resolve
                    throw Error(std::string("Selecting was failed.") + strerror(errno));
                }
            
                if (FD_ISSET(_listener.getRawSocket(), &readset))
                {
                    if (_connections.size() < _maxSimultaneousConnections)
                    {
                        _connections.emplace_back(std::make_unique<Connection>(_listener.acceptConnection()));
                    }
                }
    
            for(auto connectionIt = _connections.begin(); connectionIt != _connections.end(); ++connectionIt)
                {
                    const auto& connection = *connectionIt;
                    if(FD_ISSET(connection->getSocket().socket, &readset))
                    {
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
                                    actionIt->second(arguments, connectionIt);
                                }
                            }
                            //TODO: remove
                            std::cout<<command<<":"<<arguments<<std::endl;
                        }
                        else if(const auto* error = std::get_if<Error>(&result))
                        {
                            std::cout<<error<<std::endl;
                        }else if(const auto* closeTag = std::get_if<CloseTag>(&result))
                        {
                            closeClientConnection(connectionIt);
                        }
                    }
                }
                }
    return Error();
}

void Server::closeClientConnection(const Connections::const_iterator& it)
{
    _connections.erase(it);
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

