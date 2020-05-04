#pragma once
#include <vector>
#include <variant>
#include "SocketHolder.h"
#include "Error.h"

using DataPacket = std::string;
using ConnectionResult = std::variant<DataPacket, Error>;

class IConnection
{
public:
    virtual const SocketHolder& getSocket() const = 0;
    virtual ConnectionResult read() const = 0;
    virtual Error write(const DataPacket& data) = 0;
    
    virtual ~IConnection() {};
};
