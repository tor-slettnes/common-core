/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief C++ stream wrapper for GZip file I/O - reader
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "base.h++"

#include <istream>

namespace core::io
{
    constexpr unsigned int READ_BUFFER_SIZE = 8192;

    //--------------------------------------------------------------------------
    /// @class GZipInputBuffer
    /// @brief C++ input stream buffer that reads from a GZip file

    class GZipInputBuffer : public GZipBuffer
    {
    public:
        GZipInputBuffer(const fs::path &file_path,
                        std::size_t read_buffer_size = READ_BUFFER_SIZE);
        ~GZipInputBuffer();

        bool read_some(BufferType *buffer) override;

    private:
        const std::size_t buffer_size;
    };

    //--------------------------------------------------------------------------
    /// @class GZipInputStream
    /// @brief C++ input stream that reads from a GZip file

    class GZipInputStream : public std::istream
    {
    public:
        GZipInputStream(const fs::path &file_path);

    private:
        GZipInputBuffer input_buffer;
    };

} // namespace core::io
