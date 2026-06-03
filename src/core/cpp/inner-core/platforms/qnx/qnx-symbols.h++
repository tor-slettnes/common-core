/// -*- c++ -*-
//==============================================================================
/// @file qnx-symbols.h++
/// @brief Functions to produce symbols - QNX verison
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "posix-symbols.h++"

namespace cc::core::platform
{
    class QNXSymbolsProvider : public PosixSymbolsProvider
    {
        using This = QNXSymbolsProvider;
        using Super = PosixSymbolsProvider;

    public:
        QNXSymbolsProvider(const std::string &name = "QNXSymbolsProvider");

        std::string uuid() const noexcept override;
    };

}  // namespace cc::core::platform
