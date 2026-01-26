/// -*- c++ -*-
//==============================================================================
/// @file protobuf-version.h++
/// @brief Utility functions for `Version` message
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/protobuf/version/version.pb.h"  // generated from `variant.proto`
#include "string/format.h++"

namespace cc::protobuf
{
    using version::Version;

    Version create_version(std::uint64_t major,
                           std::uint64_t minor = 0,
                           std::uint64_t patch = 0,
                           const std::string printable_version = "");

    std::string to_string(const Version &version);

    bool is_compatible(const Version &client,
                       const Version &server,
                       bool strict = true);

    namespace version
    {
        std::ostream &operator<<(std::ostream &stream, const Version &version);
    }  // namespace version

}  // namespace cc::protobuf
