/// -*- c++ -*-
//==============================================================================
/// @file http-uploadstream.h++
/// @brief C++ stream interface for data received from HTTP server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "http-streambuffer.h++"

namespace shared::http
{
    constexpr uint SEND_QUEUE_SIZE = 1024;

    //==========================================================================
    /// UploadBuffer

    class UploadBuffer : public StreamBuffer
    {
    public:
        UploadBuffer();
        ~UploadBuffer();

    public:
        bool write_some(const std::string &buffer) override;
    };

    //==========================================================================
    /// UploadStream

    class UploadStream : public std::ostream
    {
    public:
        UploadStream();

    private:
        UploadBuffer output_buffer;
    }

}  // namespace shared::http
