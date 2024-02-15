/// -*- c++ -*-
//==============================================================================
/// @file http-streambuffer.c++
/// @brief C++ stream interface for data received from HTTP server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "http-streambuffer.h++"

namespace shared::http
{
    int StreamBuffer::sync()
    {
        return 0;
    }
}  // namespace shared::http
