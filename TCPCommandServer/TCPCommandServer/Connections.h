#pragma once
#include "IConnection.h"

#include <list>

using Connections = std::list<std::unique_ptr<IConnection>>;
