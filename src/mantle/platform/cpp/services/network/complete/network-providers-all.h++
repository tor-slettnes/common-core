// -*- c++ -*-
//==============================================================================
/// @file network-providers-all.h++
/// @brief Network service - all implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace platform::network
{
    void register_providers(bool local, const std::string &host, bool wait_for_ready);
    void unregister_providers();
}  // namespace platform::network
