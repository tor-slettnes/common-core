/// -*- c++ -*-
//==============================================================================
/// @file protobuf-version.h++
/// @brief Utility functions for `Version` message
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/protobuf/version/version.pb.h"  // generated from `variant.proto`
#include "string/format.h++"

namespace protobuf
{
    using cc::protobuf::version::Version;

    Version version(std::uint64_t major,
                    std::uint64_t minor = 0,
                    std::uint64_t patch = 0,
                    const std::string printable_version = "");

    std::string to_string(const Version &version);

    bool is_compatible(const Version &client,
                       const Version &server,
                       bool strict = true);
}  // namespace protobuf

namespace cc::protobuf::version
{
    std::ostream &operator<<(std::ostream &stream, const Version &version);
}  // namespace cc::protobuf::version
