// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-run.h++
/// @brief C++ switchboard - Launch DDS server implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "switchboard.h++"

#include <memory>

namespace switchboard::dds
{
    void run_dds_service(
        std::shared_ptr<switchboard::Provider> api_provider,
        const std::string &identity,
        int domain_id);
}
