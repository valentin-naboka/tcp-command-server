#pragma once

#include "ListenerSocket.h"
#include "Error.h"
#include "IConnection.h"

#include <unordered_map>
#include <functional>
#include "Connections.h"

using Action = std::function<bool (const std::string &arguments, const Connections::const_iterator& conn)>;
using ActionsMap = std::unordered_map<std::string, Action>;

class Server {
public:
    Server(const uint16_t port, const uint16_t maxSimultaneousConnections);
    
    void registerAction(std::string name, Action action);
    Error run();
    void closeClientConnection(const Connections::const_iterator& it);
    
private:
    int getMaxSocketValue() const;
    
private:
    ListenerSocket _listener;
    Connections _connections;
    ActionsMap _actions;
    const uint16_t _maxSimultaneousConnections;
};
