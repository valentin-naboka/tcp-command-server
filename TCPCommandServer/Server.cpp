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
    static void setupSockets(Sockets& sockets);
    static void setupSignal(const int signal);
private:
    static void handler(const int signal);
    static Sockets* _sockets;
};

Sockets* SignalHandler::_sockets = nullptr;

void SignalHandler::setupSockets(Sockets& sockets)
{
    _sockets = &sockets;
}

void SignalHandler::setupSignal(const int signal)
{
    ::signal(signal, SignalHandler::handler);
}

void SignalHandler::handler(const int signal)
{
    assert(SignalHandler::_sockets);
    SignalHandler::_sockets->clear();

    //TODO: change cout to log
    std::cout<<signal<<" signal has been caught."<<std::endl;
    exit(0);
};

Server::Server(const uint16_t port)
    : _listener(port)
{
        SignalHandler::setupSockets(_clientSockets);
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
                for(const auto& socket : _clientSockets)
                {
                    FD_SET(socket->getSocket().socket, &readset);
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
                    _clientSockets.emplace_back(std::make_unique<Connection>(_listener.acceptConnection()));
                }
    
            for(auto socketIt = _clientSockets.begin(); socketIt != _clientSockets.end(); ++socketIt)
                {
                    const auto& socket = *socketIt;
                    if(FD_ISSET(socket->getSocket().socket, &readset))
                    {
                        std::cout<<socket->getSocket().socket<<std::endl;
                        const auto& result = socket->read();
                        
                        //TODO: use std::visit
                        if(const auto* dataPacket = std::get_if<DataPacket>(&result))
                        {
                            const auto actionIt = _actions.find(*dataPacket);
                            if (actionIt != _actions.end()){
                                //TODO: parse arguments
                                actionIt->second(actionIt->first, socketIt);
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

void Server::closeClientConnection(const Sockets::const_iterator& it)
{
    _clientSockets.erase(it);
}

int Server::getMaxSocketValue() const
{
    auto maxSocketValue = _listener.getRawSocket();
    if(!_clientSockets.empty())
    {
        const auto maxIt = std::max_element(_clientSockets.cbegin(),
                                                          _clientSockets.cend(),
                                                          [](const Sockets::value_type& lhs,
                                                             const Sockets::value_type& rhs)
                                                          {
                                                              return lhs->getSocket() < rhs->getSocket();
                                                          });
        
        
        maxSocketValue = std::max((*maxIt)->getSocket().socket, maxSocketValue);
    }
    return maxSocketValue + 1;
}

