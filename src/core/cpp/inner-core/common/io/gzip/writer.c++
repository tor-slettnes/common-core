/// -*- c++ -*-
//==============================================================================
/// @file writer.c++
/// @brief C++ stream wrapper for GZip file I/O - writer
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "writer.h++"
#include "io/cutils.h++"
#include "string/format.h++"
#include "status/exception.h++"

namespace core::io
{
    //--------------------------------------------------------------------------
    // GZipOutputBuffer

    GZipOutputBuffer::GZipOutputBuffer(const fs::path &file_path,
                                       uint compression_level)
        : GZipBuffer(file_path, str::format("wb%d", compression_level).c_str())
    {
    }

    GZipOutputBuffer::~GZipOutputBuffer()
    {
        checkstatus(::gzclose_w(this->gzfile));
    }

    bool GZipOutputBuffer::write_some(const BufferType &buffer)
    {
        if (buffer.size())
        {
            int written = ::gzwrite(this->gzfile, buffer.data(), buffer.size());
            if (!written)
            {
                this->throw_last_error();
            }
            else if (written < buffer.size())
            {
                throwf(std::runtime_error,
                       "Short write to %s (attempted to write %d bytes, wrote %d bytes)",
                       this->file_path,
                       buffer.size(),
                       written);
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    //--------------------------------------------------------------------------
    // GZipOutputStream

    GZipOutputStream::GZipOutputStream(const fs::path &file_path)
        : output_buffer(file_path)
    {
        this->rdbuf(&this->output_buffer);
    }
} // namespace core::io
