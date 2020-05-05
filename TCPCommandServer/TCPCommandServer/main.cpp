//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com.
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#include "Server.h"
#include <cassert>
#include <iostream>
#include <unordered_map>

const uint16_t Port = 5678;
const uint16_t MaxSimultaneousConnections = 3;

int main(int argc, char* argv[])
{
    Server server(Port, MaxSimultaneousConnections);

    server.registerAction("version", [](const std::string& arguments, const Connections::const_iterator& connIt) {
        const auto Version = "ZSoft test task server version 1.0.\n\r";
        assert(*connIt);
        if (const auto& error = (*connIt)->write(Version)) {
            Logger::error(error);
            return false;
        }

        if (const auto& error = (*connIt)->sendAck()) {
            Logger::error(error);
        }
        return true;
    });

    server.registerAction("exit", [&server](const std::string& arguments, const Connections::const_iterator& connIt) {
        if (const auto& error = (*connIt)->sendAck()) {
            Logger::error(error);
        }
        server.closeClientConnection(connIt);
        return true;
    });

    Logger::log("Starting server...");
    if (const auto& error = server.run()) {
        Logger::error(error);
        return -1;
    }
    return 0;
}
