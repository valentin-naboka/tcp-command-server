#pragma once
#include "IConnection.h"

#include <list>
#include <memory>

using Connections = std::list<std::unique_ptr<IConnection>>;
