/// -*- c++ -*-
//==============================================================================
/// @file protobuf-version.c++
/// @brief Utility functions for `Version` message
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-version.h++"
#include "buildinfo/buildinfo.h++"

#include <iostream>

namespace cc::protobuf
{
    void populate_version(Version *version)
    {
        encode_version(
            PROJECT_VERSION_MAJOR,
            PROJECT_VERSION_MINOR,
            PROJECT_VERSION_TWEAK,
            BUILD_NUMBER,
            PROJECT_VERSION,
            version);
    }

    void encode_version(
        std::uint64_t major,
        std::uint64_t minor,
        std::uint64_t patch,
        std::uint64_t build_number,
        const std::string &printable_version,
        Version *version)
    {
        version->set_major(major);
        version->set_minor(minor);
        version->set_patch(patch);
        version->set_build_number(build_number);
        version->set_printable_version(printable_version);
    }

    Version create_version(std::uint64_t major,
                           std::uint64_t minor,
                           std::uint64_t patch,
                           std::uint64_t build_number,
                           const std::string &printable_version)
    {
        Version version;
        version.set_major(major);
        version.set_minor(minor);
        version.set_patch(patch);
        version.set_build_number(build_number);
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
            std::ostringstream ss;
            ss << version.major()
               << "."
               << version.minor()
               << "."
               << version.patch()
               << ".";

            if (version.build_number())
            {
                ss << "#" << version.build_number();
            }

            return ss.str();
        }
    }

    bool is_compatible(const Version &client,
                       const Version &server,
                       bool strict)
    {
        return ((client.major() == server.major()) &&
                ((client.minor() <= server.minor()) || !strict));
    }

    namespace version
    {
        std::ostream &operator<<(std::ostream &stream, const Version &version)
        {
            stream << cc::protobuf::to_string(version);
            return stream;
        }
    }  // namespace version

}  // namespace cc::protobuf
