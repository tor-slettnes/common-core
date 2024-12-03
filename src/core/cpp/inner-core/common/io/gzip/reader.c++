/// -*- c++ -*-
//==============================================================================
/// @file reader.c++
/// @brief C++ stream wrapper for GZip file I/O - reader
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "reader.h++"
#include "io/cutils.h++"

namespace core::io
{
    //--------------------------------------------------------------------------
    // GZipOutputBuffer

    GZipInputBuffer::GZipInputBuffer(const fs::path &file_path,
                                     std::size_t buffer_size)
        : GZipBuffer(file_path, "rb"),
          buffer_size(buffer_size)
    {
    }

    GZipInputBuffer::~GZipInputBuffer()
    {
        checkstatus(::gzclose_r(this->gzfile));
    }

    bool GZipInputBuffer::read_some(BufferType *buffer)
    {
        buffer->resize(this->buffer_size);
        int nchars = ::gzread(this->gzfile, buffer, buffer->size());
        if (nchars >= 0)
        {
            buffer->resize(nchars);
            return (nchars > 0);
        }
        else
        {
            this->throw_last_error();
            return false;
        }
    }

    //--------------------------------------------------------------------------
    // GZipInputStream

    GZipInputStream::GZipInputStream(const fs::path &file_path)
        : input_buffer(file_path)
    {
        this->rdbuf(&this->input_buffer);
    }

} // namespace core::io
