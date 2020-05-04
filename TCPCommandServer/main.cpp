#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cstdarg>

#include "Server.h"
#include <iostream>
#include <unordered_map>

int main(int argc , char *argv[])
{
    Server server(4000);
    
    server.registerAction("version", [](const std::string &arguments,
                                        const Connections::const_iterator& connIt)
                                    {
                                        const auto Version = "ZSoft test task server version 1.0.\n\r";
                                        assert(*connIt);
                                        (*connIt)->write(Version);
                                        return true;
                                    }
                          );

    server.registerAction("exit", [&server](const std::string &arguments,
                                            const Connections::const_iterator& connIt)
                                    {
                                        server.closeClientConnection(connIt);
                                        return true;
                                    }
                          );

    server.run();
    return 0;
}
    
