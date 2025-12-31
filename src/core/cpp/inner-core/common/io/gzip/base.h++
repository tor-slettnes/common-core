/// -*- c++ -*-
//==============================================================================
/// @file base.h++
/// @brief C++ stream wrapper for GZip file I/O - base
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "io/streambuffer.h++"
#include "types/filesystem.h++"

#include "zlib.h"

namespace core::io
{
    constexpr unsigned int GZIP_BUFFER_SIZE = 65536;

    //--------------------------------------------------------------------------
    /// @class GZipBuffer
    /// @brief C++ stream buffer that reads from or writes to a GZip file

    class GZipBuffer : public StreamBuffer
    {
    protected:
        GZipBuffer(const fs::path &file_path,
                   const std::string &open_mode);

    protected:
        void throw_last_error() const;

    protected:
        fs::path file_path;
        gzFile gzfile;
    };

} // namespace core::io
