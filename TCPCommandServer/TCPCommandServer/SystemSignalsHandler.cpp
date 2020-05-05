//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com. 
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#include "SystemSignalsHandler.h"
#include "Logger.h"

#include <sstream>
#include <csignal>
#include <cassert>

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

    std::stringstream ss;
    ss<<signal<<" signal has been caught."<<std::endl<<"Server has been stopped.";
    Logger::log(ss.str());
    exit(0);
};
