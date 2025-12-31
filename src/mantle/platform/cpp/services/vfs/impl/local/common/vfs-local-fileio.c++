// -*- c++ -*-
//==============================================================================
/// @file vfs-local-fileio.c++
/// @brief VFS service - reader/writer
/// @author Tor Slettnes
//==============================================================================

#include "vfs-local-fileio.h++"

namespace vfs::local
{
    //==========================================================================
    // FileReader

    FileReader::FileReader(const Location &loc,
                           const std::ios::openmode mode)
        : std::ifstream(loc.localPath(), mode),
          loc(loc)
    {
        logf_debug("FileReader opened for location %s", loc);
    }

    //==========================================================================
    // FileWriter

    FileWriter::FileWriter(const Location &loc,
                           const std::ios::openmode mode)
        : std::ofstream(loc.localPath(), mode),
          loc(loc)
    {
        logf_debug("FileWriter opened for location %s", loc);
    }
}  // namespace vfs::local
