#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cstdarg>

#include "Server.h"
#include <iostream>
#include <unordered_map>

int main(int argc , char *argv[])
{
    //TODO: change port
    Server server(4000, 3);
    
    server.registerAction("version", [](const std::string &arguments,
                                        const Connections::const_iterator& connIt)
                                    {
                                        const auto Version = "ZSoft test task server version 1.0.\n\r";
                                        assert(*connIt);
                                        if(const auto& error = (*connIt)->write(Version))
                                        {
                                            Logger::error(error);
                                            return false;
                                        }
        
                                        if(const auto& error = (*connIt)->sendAck())
                                        {
                                            Logger::error(error);
                                        }
                                        return true;
                                    }
                          );

    server.registerAction("exit", [&server](const std::string &arguments,
                                            const Connections::const_iterator& connIt)
                                    {
                                        if(const auto& error = (*connIt)->sendAck())
                                        {
                                            Logger::error(error);
                                        }
                                        server.closeClientConnection(connIt);
                                        return true;
                                    }
                          );

    Logger::log("Starting server...");
    if(const auto& error = server.run())
    {
        Logger::error(error);
        return -1;
    }
    return 0;
}
    
