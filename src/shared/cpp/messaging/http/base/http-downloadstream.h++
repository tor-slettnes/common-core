/// -*- c++ -*-
//==============================================================================
/// @file http-downloadstream.h++
/// @brief C++ stream interface for data received from HTTP server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "http-streambuffer.h++"

namespace shared::http
{
    constexpr uint RECEIVE_QUEUE_SIZE = 1024;

    //==========================================================================
    /// DownloadBuffer

    class DownloadBuffer : public StreamBuffer
    {
    public:
        DownloadBuffer();

    public:
        void feed_some(std::string buffer);
        bool read_some(std::string *buffer) override;

    private:
        shared::types::BlockingQueue receive_queue;
    };

    //==========================================================================
    /// DownloadStream

    class DownloadStream : public std::istream
    {
    public:
        DownloadStream();

    private:
        DownloadBuffer input_buffer;
    };

}  // namespace shared::http
