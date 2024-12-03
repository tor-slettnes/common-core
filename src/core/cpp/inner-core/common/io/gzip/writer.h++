/// -*- c++ -*-
//==============================================================================
/// @file writer.h++
/// @brief C++ stream wrapper for GZip file I/O - writer
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "base.h++"

#include <ostream>

namespace core::io
{
    //--------------------------------------------------------------------------
    /// @class GZipOutputBuffer
    /// @brief C++ output stream buffer that writes to a GZip file

    class GZipOutputBuffer : public GZipBuffer
    {
    public:
        GZipOutputBuffer(const fs::path &file_path,
                         uint compression_level = 6);
        ~GZipOutputBuffer();

        bool write_some(const BufferType &buffer) override;
    };

    //--------------------------------------------------------------------------
    /// @class GZipOutputStream
    /// @brief C++ output stream that reads from a GZip file

    class GZipOutputStream : public std::ostream
    {
    public:
        GZipOutputStream(const fs::path &file_path);

    private:
        GZipOutputBuffer output_buffer;
    };

} // namespace core::io
