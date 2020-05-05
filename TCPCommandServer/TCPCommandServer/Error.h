//-----------------------------------------------------------------------------
// Copyright (c) 2020, Valentin Naboka, valentin.naboka@gmail.com.
// The sources are under MIT license.
//-----------------------------------------------------------------------------

#pragma once

#include <string>

class Error {
public:
    Error() = default;
    Error(std::string error);

    operator std::string() const;
    operator bool() const;

private:
    std::string _errorMsg;
};

std::string wrapErrorno(std::string description);
