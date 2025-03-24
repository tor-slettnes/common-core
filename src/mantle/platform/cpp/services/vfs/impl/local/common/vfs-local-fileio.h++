// -*- c++ -*-
//==============================================================================
/// @file vfs-local-fileio.h++
/// @brief VFS service - reader/writer
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-location.h++"
#include <fstream>

namespace vfs::local
{
    //==========================================================================
    // FileReader

    class FileReader : public std::ifstream
    {
    public:
        FileReader(const Location &loc,
                   const std::ios::openmode mode = std::ios::in | std::ios::binary);

    private:
        Location loc;
    };

    //==========================================================================
    // FileReader

    class FileWriter : public std::ofstream
    {
    public:
        FileWriter(const Location &loc,
                   const std::ios::openmode mode = std::ios::out | std::ios::binary);

    private:
        Location loc;
    };
}  // namespace vfs::local
