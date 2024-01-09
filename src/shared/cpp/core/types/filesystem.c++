/// -*- c++ -*-
//==============================================================================
/// @file filesystem.c++
/// @brief Path-related types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "filesystem.h++"

namespace std::filesystem
{
    static std::unordered_map<file_type, std::string> typenames = {
        {file_type::none, "none"},
        {file_type::not_found, "not_found"},
        {file_type::regular, "regular"},
        {file_type::directory, "directory"},
        {file_type::symlink, "symlink"},
        {file_type::block, "block"},
        {file_type::character, "character"},
        {file_type::fifo, "fifo"},
        {file_type::socket, "socket"},
        {file_type::unknown, "unknown"},
    };

    std::ostream &operator<<(std::ostream &stream, const file_type &type)
    {
        try
        {
            stream << typenames.at(type);
        }
        catch (const std::out_of_range &)
        {
            stream << typenames.at(file_type::unknown);
        }
        return stream;
    }
}  // namespace std::filesystem
