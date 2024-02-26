/// -*- c++ -*-
//==============================================================================
/// @file logging.c++
/// @brief Message capture
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logging.h++"

namespace core::logging
{
    SyncDispatcher message_dispatcher;
    AsyncDispatcher structured_dispatcher;
}
