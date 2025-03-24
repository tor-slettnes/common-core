// -*- c++ -*-
//==============================================================================
/// @file sysconfig-process.c++
/// @brief SysConfig service - Process invocation API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-process.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

#include <system_error>

namespace sysconfig
{
    //==========================================================================
    // Helper methods for this module

    PortableExitStatus::PortableExitStatus(bool success,
                                           int exit_code,
                                           int exit_signal,
                                           const std::string &symbol,
                                           const std::string &text)
        : success_(success),
          code_(exit_code),
          signal_(exit_signal),
          symbol_(symbol),
          text_(text)
    {
    }

    int PortableExitStatus::exit_code() const
    {
        return this->code_;
    }

    int PortableExitStatus::exit_signal() const
    {
        return this->signal_;
    }

    bool PortableExitStatus::success() const
    {
        return this->success_;
    }

    std::string PortableExitStatus::symbol() const
    {
        return this->symbol_;
    }

    std::string PortableExitStatus::text() const
    {
        return this->text_;
    }

    core::platform::ProviderProxy<ProcessInterface> process("process");
}  // namespace sysconfig
