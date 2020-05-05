#include "SystemSignalsHandler.h"

//TODO: remove
#include <iostream>

Connections* SignalHandler::_connections = nullptr;

void SignalHandler::setupConnections(Connections& connections)
{
    _connections = &connections;
}

void SignalHandler::setupSignal(const int signalCode)
{
    signal(signalCode, SignalHandler::handler);
}

void SignalHandler::handler(const int signal)
{
    assert(SignalHandler::_connections);
    SignalHandler::_connections->clear();

    //TODO: change cout to log
    std::cout<<signal<<" signal has been caught."<<std::endl;
    exit(0);
};
