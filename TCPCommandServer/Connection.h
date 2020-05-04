//
//  Connection.hpp
//  MysqlProxy
//
//  Created by Valentin on 5/4/20.
//  Copyright © 2020 Valentin. All rights reserved.
//

#pragma once
#include "IConnection.h"

class Connection final : public IConnection
{
public:
    Connection(SocketHolder socket);
    
    const SocketHolder& getSocket() const override;
    ConnectionResult read() const override;
    Error write(const DataPacket& data) override;
    
    ~Connection() override;
private:
    SocketHolder _socketHolder;
};
