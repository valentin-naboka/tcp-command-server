//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com.
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#include "Error.h"

#include <cstring>
#include <sstream>

Error::Error(std::string error)
    : _errorMsg(std::move(error))
{
}

Error::operator std::string() const
{
    return _errorMsg;
}

Error::operator bool() const
{
    return !_errorMsg.empty();
}

std::string wrapErrorno(std::string description)
{
    std::stringstream ss;
    ss << std::move(description) << std::strerror(errno);
    return ss.str();
}
