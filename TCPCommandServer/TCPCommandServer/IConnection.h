//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com. 
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#pragma once
#include <vector>
#include <variant>
#include "SocketHolder.h"
#include "Error.h"

using DataPacket = std::string;
struct CloseTag{};
using ConnectionResult = std::variant<DataPacket, Error, CloseTag>;

class IConnection
{
public:
    virtual const SocketHolder& getSocket() const = 0;
    
    virtual ConnectionResult read() const = 0;
    virtual Error write(const DataPacket& data) = 0;
    
    virtual Error sendAck() = 0;
    virtual Error sendError() = 0;
    
    virtual ~IConnection() {};
};
