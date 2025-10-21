/// -*- c++ -*-
//==============================================================================
/// @file protobuf-version.c++
/// @brief Utility functions for `Version` message
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-version.h++"

#include <iostream>

namespace protobuf
{
    Version version(std::uint64_t major,
                    std::uint64_t minor,
                    std::uint64_t patch,
                    const std::string &printable_version)
    {
        Version version;
        version.set_major(major);
        version.set_minor(minor);
        version.set_patch(patch);
        version.set_printable_version(printable_version);
        return version;
    }

    std::string to_string(const Version &version)
    {
        if (!version.printable_version().empty())
        {
            return version.printable_version();
        }
        else
        {
            return core::str::format(
                "%d.%d.%d",
                version.major(),
                version.minor(),
                version.patch());
        }
    }

    bool is_compatible(const Version &client,
                       const Version &server,
                       bool strict)
    {
        return ((client.major() == server.major()) &&
                ((client.minor() <= server.minor()) || !strict));
    }
}  // namespace protobuf

namespace cc::protobuf::version
{
    std::ostream &operator<<(std::ostream &stream, const Version &version)
    {
        stream << ::protobuf::to_string(version);
        return stream;
    }
}  // namespace cc::protobuf::version
