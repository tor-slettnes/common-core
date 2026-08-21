/// -*- c++ -*-
//==============================================================================
/// @file qnx-path.h++
/// @brief Path-related functions - QNX specifics
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "posix-path.h++"

namespace cc::core::platform
{
    class QNXPathProvider : public PosixPathProvider
    {
        using This = QNXPathProvider;
        using Super = PosixPathProvider;

    public:
        QNXPathProvider(const fs::path &exec_path);
        fs::path exec_path() const noexcept override;
    };

}  // namespace cc::core::platform
