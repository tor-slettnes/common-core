/// -*- c++ -*-
//==============================================================================
/// @file http-uploadstream.c++
/// @brief C++ stream interface for data received from HTTP server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "http-uploadstream.h++"

namespace shared::http
{
    //==========================================================================
    /// UploadBuffer

    UploadBuffer::UploadBuffer()
    {
    }

    UploadBuffer::~UploadBuffer()
    {
    }

    bool UploadBuffer::write_some(const std::string &buffer)
    {
    }

    //==========================================================================
    /// UploadStream

    UploadStream::UploadStream()
    {
        this->rdbuf(&this->output_buffer);
    }

}  // namespace shared::http
