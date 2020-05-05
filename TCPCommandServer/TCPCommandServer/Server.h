//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com.
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#pragma once
#include "Error.h"
#include "IConnection.h"
#include "ListenerSocket.h"
#include "Connections.h"

#include <functional>
#include <unordered_map>

using Action = std::function<bool(const std::string& arguments, const Connections::const_iterator& conn)>;
using ActionsMap = std::unordered_map<std::string, Action>;

class Server {
public:
    Server(const uint16_t port, const uint16_t maxSimultaneousConnections);

    void registerAction(std::string name, Action action);
    Error run();
    void closeClientConnection(const Connections::const_iterator& it);

private:
    void handleListenerScoket();
    void handleClientConnection(const Connections::const_iterator& connIt);
    int getMaxSocketValue() const;

private:
    ListenerSocket _listener;
    Connections _connections;
    ActionsMap _actions;
    const uint16_t _maxSimultaneousConnections;
};
