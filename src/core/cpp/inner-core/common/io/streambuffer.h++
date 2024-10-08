/// -*- c++ -*-
//==============================================================================
/// @file streambuffer.h++
/// @brief C++ stream buffer interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <streambuf>
#include <vector>
#include <utility>

namespace core::io
{
    // Output buffer size allocated by `overflow()`
    constexpr std::streamsize BUFSIZE = 4096;

    //==========================================================================
    /// @class StreamBuffer
    /// @brief streambuf implementation

    class StreamBuffer : public std::streambuf
    {
    protected:
        using BufferType = std::string;

    protected:
        /// Virtual method to read a new chunk into our bytes buffer.
        /// @param[out] buffer
        ///     buffer to populate
        /// @return
        ///     Number of characters that was read
        virtual inline bool read_some(BufferType *buffer) { return false; }

        /// Virtual method to read a new chunk into our string buffer.
        /// @param[in] buffer
        ///     buffer to populate
        /// @return
        ///     Number of characters that was written
        virtual inline bool write_some(const BufferType &buffer) { return false; }

    protected:
        // buffer management
        int sync() override;

        // get area
        std::streamsize showmanyc() override;
        std::streamsize xsgetn(char_type *__s, std::streamsize __n) override;
        int_type underflow() override;
        int_type pbackfail(int_type __c = traits_type::eof()) override;

        // put area
        std::streamsize xsputn(const char_type *__s, std::streamsize __n) override;
        int_type overflow(int_type __c = traits_type::eof()) override;

    private:
        BufferType __input;
        bool __input_completed = false;
    };
}  // namespace core::io
