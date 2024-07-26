// -*- c++ -*-
//==============================================================================
/// @file vfs-types.c++
/// @brief VFS service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-types.h++"

namespace platform::vfs
{
    std::ostream &operator<<(std::ostream &stream, const OperationFlags &flags)
    {
        std::vector<std::string> strings = {};
        if (flags.inside_target)
        {
            strings.push_back("inside_target");
        }

        if (flags.force)
        {
            strings.push_back("force");
        }

        if (flags.dereference)
        {
            strings.push_back("dereference");
        }

        if (flags.merge)
        {
            strings.push_back("merge");
        }

        if (flags.update)
        {
            strings.push_back("update");
        }

        if (flags.with_attributes)
        {
            strings.push_back("with_attributes");
        }

        if (flags.include_hidden)
        {
            strings.push_back("include_hidden");
        }

        if (flags.ignore_case)
        {
            strings.push_back("ignore_case");
        }

        stream << "flags=" << strings;
        return stream;
    }
}  // namespace platform::vfs
