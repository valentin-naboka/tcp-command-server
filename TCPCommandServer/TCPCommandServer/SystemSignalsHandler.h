#pragma once

#include "Connections.h"

class SignalHandler final
{
public:
    static void setupConnections(Connections& connections);
    static void setupSignal(const int signal);
private:
    static void handler(const int signal);
    static Connections* _connections;
};
