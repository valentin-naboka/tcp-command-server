//
//  Server.cpp
//  MysqlProxy
//
//  Created by Valentin on 5/4/20.
//  Copyright © 2020 Valentin. All rights reserved.
//

#include "Server.h"
#include "Connection.h"

#include <variant>

//TODO: remove
#include <iostream>

class SignalHandler final
{
public:
    static void setupConnections(Connections& connections);
    static void setupSignal(const int signal);
private:
    static void handler(const int signal);
    static Connections* _connections;
};

Connections* SignalHandler::_connections = nullptr;

void SignalHandler::setupConnections(Connections& connections)
{
    _connections = &connections;
}

void SignalHandler::setupSignal(const int signal)
{
    ::signal(signal, SignalHandler::handler);
}

void SignalHandler::handler(const int signal)
{
    assert(SignalHandler::_connections);
    SignalHandler::_connections->clear();

    //TODO: change cout to log
    std::cout<<signal<<" signal has been caught."<<std::endl;
    exit(0);
};

Server::Server(const uint16_t port)
    : _listener(port)
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
    
                //TODO: resolve max sock value
                if(select(getMaxSocketValue(), &readset, nullptr, nullptr, &timeout) < 0)
                {
                    //TODO: resolve
                    throw Error(std::string("Selecting was failed.") + strerror(errno));
                }
            
                if (FD_ISSET(_listener.getRawSocket(), &readset))
                {
                    _connections.emplace_back(std::make_unique<Connection>(_listener.acceptConnection()));
                }
    
            for(auto connectionIt = _connections.begin(); connectionIt != _connections.end(); ++connectionIt)
                {
                    const auto& connection = *connectionIt;
                    if(FD_ISSET(connection->getSocket().socket, &readset))
                    {
                        std::cout<<connection->getSocket().socket<<std::endl;
                        const auto& result = connection->read();
                        
                        //TODO: use std::visit
                        if(const auto* dataPacket = std::get_if<DataPacket>(&result))
                        {
                            const auto actionIt = _actions.find(*dataPacket);
                            if (actionIt != _actions.end()){
                                //TODO: parse arguments
                                actionIt->second(actionIt->first, connectionIt);
                            }
                            std::cout<<*dataPacket<<std::endl;
                        }
                        else if(const auto* error = std::get_if<Error>(&result))
                        {
                            std::cout<<error<<std::endl;
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

