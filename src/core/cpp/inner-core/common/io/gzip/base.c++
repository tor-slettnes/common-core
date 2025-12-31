/// -*- c++ -*-
//==============================================================================
/// @file base.c++
/// @brief C++ stream wrapper for GZip file I/O - base
/// @author Tor Slettnes
//==============================================================================

#include "base.h++"
#include "io/cutils.h++"

namespace core::io
{
    //--------------------------------------------------------------------------
    // GZipBuffer

    GZipBuffer::GZipBuffer(const fs::path &file_path,
                           const std::string &open_mode)
        : file_path(file_path),
          gzfile(checkstatus(::gzopen(file_path.c_str(), open_mode.c_str())))
    {
        checkstatus(::gzbuffer(this->gzfile, GZIP_BUFFER_SIZE));
    }

    void GZipBuffer::throw_last_error() const
    {
        int errnum = 0;
        std::string message = ::gzerror(this->gzfile, &errnum);
        if (errnum == Z_ERRNO)
        {
            throw std::system_error(errno, std::generic_category(), "GZipBuffer");
        }
        else
        {
            throw std::runtime_error(message);
        }
    }

} // namespace core::io
