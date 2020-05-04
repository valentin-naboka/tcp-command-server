//#include "Exceptions.h"
//#include "Proxy.h"
//#include "Logger.h"
//#include "ServerState.h"
//
//#include <stdio.h>
//#include <iostream>
//#include <thread>

//const char* const MySQLServerIp = "127.0.0.1";
//uint16_t MySQLServerPort = 3306;
//uint16_t ProxyPort = 3305;

//const char* const PathToLogFile = "Log.txt";

#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cstdarg>

#include "Server.h"
#include <iostream>
#include <unordered_map>

//using Sockets = std::vector<int>;
//class SignalHandler final
//{
//public:
//    static void setupSockets(Sockets& sockets);
//    static void setupSignal(const int signal);
//private:
//    static void handler(const int signal);
//    static Sockets* _sockets;
//};
//
//Sockets* SignalHandler::_sockets = nullptr;
//
//void SignalHandler::setupSockets(Sockets& sockets)
//{
//    _sockets = &sockets;
//}
//
//void SignalHandler::setupSignal(const int signal)
//{
//    ::signal(signal, SignalHandler::handler);
//}
//
//void SignalHandler::handler(const int signal)
//{
//    assert(SignalHandler::_sockets);
//    SignalHandler::_sockets->clear();
////    for (const auto& s : *SignalHandler::_sockets)
////    {
////        close(s);
////        std::cout<<std::endl<<s<<std::endl;
////        //TODO: change cout to log
////    }
//    std::cout<<signal<<" signal has been caught."<<std::endl;
//    exit(0);
//};

//using RespondDataQueue = std::unordered_map<int, std::queue<std::string>>;
//using Action = bool (*)(const std::string &arguments, int client_socket);
//using ActionsMap = std::unordered_map<std::string, Action>;

int main(int argc , char *argv[])
{
    Server server(4000);
    
    server.registerAction("version", [](const std::string &arguments, const Sockets::const_iterator& connIt)
                                    {
                                        const auto Version = "ZSoft test task server version 1.0.\n\r";
                                        assert(*connIt);
                                        (*connIt)->write(Version);
                                        return true;
                                    }
                          );

    server.registerAction("exit", [&server](const std::string &arguments, const Sockets::const_iterator& connIt)
                                    {
                                        server.closeClientConnection(connIt);
                                        return true;
                                    }
                          );

    server.run();
//    ActionsMap actions;
//    actions.emplace(std::make_pair("version", [](const std::string &arguments, int clientSocket)
//                                                {
//                                                    const auto Version = "ZSoft test task server version 1.0.";
//                                                    const auto dataLen = std::char_traits<char>::length(Version);
//                                                    ssize_t bSent = 0;
//                                                    while(bSent != dataLen)
//                                                    {
//                                //                        std::cout<<"bread is: "<<bread<<std::endl;
//                                //                        std::fill(buf, buf+1400, '\0');
//                                                        //ssize_t bread = 1;
//                                            //            while(bread != 0 && bread != -1)
//                                            //            {
//                                                            const auto numSent = send(clientSocket, Version+bSent, dataLen - bSent, 0);
//                                                            if(numSent == -1)
//                                                            {
//                                                                //error
//                                                            }
//                                                            bSent += numSent;
//
//                                                            //std::cout<<buf;
//                                            //            }
//                                                        //std::cout<<std::string(buf, endOfCommand);
//                                                    }
//
//                                                    return true;
//                                                }));
//
//    actions.emplace(std::make_pair("exit", [](const std::string &arguments, int clientSocket)
//                        {
//                            close(clientSocket);
//                            return true;
//                        }));
//
//    Sockets socks;
//    int listener = -1;
//    sockaddr_in addr;
//    char buf[1400];
//    listener = socket(AF_INET, SOCK_STREAM, 0);
//    socks.push_back(listener);
//    fcntl(listener, F_SETFL, O_NONBLOCK);
//    if(listener < 0)
//    {
//        std::cout<<strerror(errno);
//        exit(1);
//    }
//
//    addr.sin_family = AF_INET;
//    addr.sin_port = htons(4000);
//    addr.sin_addr.s_addr = INADDR_ANY;
//    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))<0)
//    {
//        std::cout<<strerror(errno);
//        exit(1);
//    }
//
//    if (listen(listener, 3) != 0)
//    {
//        std::cout<<strerror(errno);
//        exit(1);
//    }
//
//    SignalHandler::setupSockets(socks);
//    SignalHandler::setupSignal(SIGINT);
//    SignalHandler::setupSignal(SIGTERM);
//    SignalHandler::setupSignal(SIGKILL);
//
//    //////////////////////////////
//    RespondDataQueue respondQueue;
//    while(true)
//    {
//            fd_set readset;
//            FD_ZERO(&readset);
//
//            fd_set writeset;
//            FD_ZERO(&writeset);
//
//            for(const auto socket : socks)
//            {
//                FD_SET(socket, &readset);
//                FD_SET(socket, &writeset);
//            }
//
//            timeval timeout;
//            timeout.tv_sec = 60;
//            timeout.tv_usec = 0;
//
//            //TODO: resolve max sock value
//            if(select(socks.back() + 1, &readset, &writeset, nullptr, &timeout) < 0)
//            {
//                //TODO: resolve
//                throw Error(std::string("Selecting was failed.") + strerror(errno));
//            }
//
//            for(const auto socket : socks)
//            {
//                if(FD_ISSET(socket, &readset))
//                {
//                    std::cout<<std::endl<<socket<<std::endl;
//                    if(socket == socks[0])
//                    {
//                        const auto dataSock = accept(listener, nullptr, nullptr);
//                        if (dataSock > 0)
//                        {
//                            int flags = 0;
//                            if ((flags = fcntl(dataSock, F_GETFL, 0)) < 0)
//                            {
//                                //TODO: resolve
//                                throw Error("Couldn't get socket flags.");
//                            }
//
//                            if (fcntl(dataSock, F_SETFL, flags | O_NONBLOCK) < 0)
//                            {
//                                //TODO: resolve
//                                throw Error("Couldn't set socket flags.");
//                            }
//                            socks.push_back(dataSock);
//                            respondQueue[dataSock];
//                        }
//                        else
//                        {
//                            std::cout<<strerror(errno);
//                        }
//                    }
//                    else
//                    {
//                        size_t endOfCommand = 0;
//                        //while(std::string(buf, endOfCommand) != "exit")
//                        ssize_t bread = 1;
//                        while(bread > 0)
//                        {
//                            std::cout<<"bread is: "<<bread<<std::endl;
//                            std::fill(buf, buf+1400, '\0');
//                            //ssize_t bread = 1;
//                //            while(bread != 0 && bread != -1)
//                //            {
//                                bread = recv(socket, buf, 1400, 0);
//                                //std::cout<<buf;
//                //            }
//                            if(bread > 1)
//                            {
//                                endOfCommand = bread - 2;
//                                const auto socketDataIt = respondQueue.find(socket);
//                                if(socketDataIt != respondQueue.end())
//                                {
//                                    socketDataIt->second.push(std::string(buf, endOfCommand));
//                                }
//                            }
//
//                            std::cout<<std::string(buf, endOfCommand);
//                            const auto actionIt = actions.find(std::string(buf, endOfCommand));
//                            if (actionIt != actions.end()){
//                                //TODO: parse arguments
//                                actionIt->second("", socket);
//                            }
//                        }
//                    }
//
//                }
//
//    //            if(FD_ISSET(socket, &writeset))
//    //            {
//    //                 if(socket != socks[0])
//    //                 {
//    //                     const auto socketDataIt = respondQueue.find(socket);
//    //                     if(socketDataIt != respondQueue.end() && socketDataIt->second.)
//    //                     {
//    //                         const auto* msg = socketDataIt->second.front().data();
//    //
//    //                         std::cout<<"write socket is: "<<socket<<std::endl;
//    //                         std::cout<<"write data is: "<<msg<<std::endl;
//    //
//    //                         ssize_t bSent = 0;
//    //                        ssize_t dataLen = socketDataIt->second.front().size();
//    //                        while(bSent == dataLen)
//    //                        {
//    //    //                        std::cout<<"bread is: "<<bread<<std::endl;
//    //    //                        std::fill(buf, buf+1400, '\0');
//    //                            //ssize_t bread = 1;
//    //                //            while(bread != 0 && bread != -1)
//    //                //            {
//    //                                const auto numSent = send(socket, msg+bSent, dataLen - bSent, 0);
//    //                                if(numSent == -1)
//    //                                {
//    //                                    //error
//    //                                }
//    //                            bSent += numSent;
//    //
//    //                                //std::cout<<buf;
//    //                //            }
//    //                            //std::cout<<std::string(buf, endOfCommand);
//    //                        }
//    //                         socketDataIt->second.pop();
//    //                    }
//    //                 }
//    //            }
//            }
//
//
//    //        const auto dataSock = accept(listener, nullptr, nullptr);
//    //        if (dataSock > 0)
//    //        {
//    //            socks.push_back(dataSock);
//    //            //TODO: refactor
//    //            size_t endOfCommand = 0;
//    //            while(std::string(buf, endOfCommand) != "exit")
//    //            {
//    //                std::fill(buf, buf+1400, '\0');
//    //                ssize_t bread = 1;
//    //    //            while(bread != 0 && bread != -1)
//    //    //            {
//    //                    bread = recv(dataSock, buf, 1400, 0);
//    //                    //std::cout<<buf;
//    //    //            }
//    //                if(bread > 1)
//    //                {
//    //                    endOfCommand = bread - 2;
//    //                }
//    //                std::cout<<std::string(buf, endOfCommand);
//    //            }
//    //        }
//            }
    /////////////////////////////

//    close(listener);
//    for (const auto socket : socks)
//    {
//        close(socket);
//    }
    
//    signal(SIGINT, [&socks](const int signum){
//        for(const auto s : socks)
//        {
//            close(s)
//        }
//        close(listener);
//        
//        std::cout<<signum<<"signal has been cauth."<<std::endl;
//        exit(0);
//    });
    
//    signal(SIGKILL, [](const int signum){
//        std::cout<<signum<<"signal has been cauth."<<std::endl;
//        exit(0);
//    });
//
//    signal(SIGTERM, [](int signum){
//        std::cout<<signum<<" signal has been cauth."<<std::endl;
//        return exit(0);
//    });
//
//    while(true)
//    {
//        sleep(1);
//        std::cout<<"Running"<<std::endl;
//    }
//    getchar();
    return 0;
}
    
