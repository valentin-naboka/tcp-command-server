//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com.
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#pragma once
#include "Error.h"
#include "SocketHolder.h"

#include <variant>

using ListenerSocketResult = std::variant<SocketHolder, Error>;

class ListenerSocket final {
public:
    ListenerSocket(const uint16_t port, const uint16_t maxSimultaneousConnections);

    Error init();

    ListenerSocketResult acceptConnection() const;
    int getRawSocket() const;

private:
    SocketHolder _listener;
    const uint16_t _port;
    const uint16_t _maxSimultaneousConnections;
};
