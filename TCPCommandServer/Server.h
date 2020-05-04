//
//  Server.hpp
//  MysqlProxy
//
//  Created by Valentin on 5/4/20.
//  Copyright © 2020 Valentin. All rights reserved.
//

#pragma once

#include "ListenerSocket.h"
#include "Error.h"
#include "IConnection.h"

#include <list>
#include <unordered_map>
#include <functional>

using Connections = std::list<std::unique_ptr<IConnection>>;
using Action = std::function<bool (const std::string &arguments, const Connections::const_iterator& conn)>;
using ActionsMap = std::unordered_map<std::string, Action>;

class Server {
public:
    Server(const uint16_t port);
    
    void registerAction(std::string name, Action action);
    Error run();
    void closeClientConnection(const Connections::const_iterator& it);
    
private:
    int getMaxSocketValue() const;
    
private:
    ListenerSocket _listener;
    Connections _connections;
    ActionsMap _actions;
};
