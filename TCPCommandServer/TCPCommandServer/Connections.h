//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com. 
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#pragma once
#include "IConnection.h"

#include <list>
#include <memory>

using Connections = std::list<std::unique_ptr<IConnection>>;
