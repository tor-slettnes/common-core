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

namespace platform::sysconfig
{
    // //==========================================================================
    // // Helper methods for this module

    // void print_first_line(std::ostream &stream,
    //                       const std::string &text,
    //                       const std::string &prefix,
    //                       const std::string &continuation = "...",
    //                       const std::string &posfix = "")
    // {
    //     std::vector<std::string> lines = core::str::split(text, "\n", 1);
    //     if (!lines.empty())
    //     {
    //         stream << prefix
    //                << lines.front();

    //         if (lines.size() > 1)
    //         {
    //             stream << continuation;
    //         }
    //     }
    // }

    // //==========================================================================
    // // CommandInvocation

    // void CommandInvocation::to_stream(std::ostream &stream) const
    // {
    //     core::str::format(
    //         stream,
    //         "{cwd=%s, argv=%s}",
    //         this->working_directory,
    //         this->argv);
    // }

    // //==========================================================================
    // // CommandInvocation

    // void CommandContinuation::to_stream(std::ostream &stream) const
    // {
    //     stream << "{pid=" << this->pid;
    //     print_first_line(stream, this->input, ", input=");
    //     stream << "}";
    // }

    // //==========================================================================
    // // CommandResponse

    // void CommandResponse::to_stream(std::ostream &stream) const
    // {
    //     stream << "{exit_status=" << this->exit_status;
    //     print_first_line(stream, this->stdout, ", stdout=");
    //     print_first_line(stream, this->stderr, ", stderr=");
    //     stream << "}";
    // }

    core::platform::ProviderProxy<ProcessInterface> process("process");
}  // namespace platform::sysconfig
