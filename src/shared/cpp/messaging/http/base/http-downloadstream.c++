/// -*- c++ -*-
//==============================================================================
/// @file http-downloadstream.c++
/// @brief C++ stream interface for data received from HTTP server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "http-downloadstream.h++"

namespace shared::http
{
    //==========================================================================
    /// DownloadBuffer

    DownloadBuffer::DownloadBuffer()
        : receive_queue(RECEIVE_QUEUE_SIZE, shared::types::OverflowDisposition::BLOCK)
    {
    }

    void DownloadBuffer::feed_some(std::string &buffer)
    {
        this->receive_queue.put(buffer);
    }

    bool DownloadBuffer::read_some(std::string *buffer)
    {
        buffer->assign(this->receive_queue.get());
        return !buffer->empty();
    }

    //==========================================================================
    /// DownloadStream

    DownloadStream::DownloadStream()
    {
        this->rdbuf(&this->input_buffer);
    }

}  // namespace shared::http
