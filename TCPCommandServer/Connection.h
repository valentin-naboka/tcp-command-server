#pragma once
#include "IConnection.h"

class Connection final : public IConnection
{
public:
    Connection(SocketHolder socket);
    
    const SocketHolder& getSocket() const override;
    ConnectionResult read() const override;
    Error write(const DataPacket& data) override;
    
    Error sendAck() override;
    Error sendError() override;
    
    ~Connection() override = default;
private:
    SocketHolder _socketHolder;
};
