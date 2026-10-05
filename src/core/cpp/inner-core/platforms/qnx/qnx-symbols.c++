/// -*- c++ -*-
//==============================================================================
/// @file qnx-symbols.c++
/// @brief Functions to produce symbols - QNX verison
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "qnx-symbols.h++"

namespace cc::core::platform
{
    QNXSymbolsProvider::QNXSymbolsProvider(const std::string &name)
        : PosixSymbolsProvider(name)
    {
    }
};

}  // namespace cc::core::platform
