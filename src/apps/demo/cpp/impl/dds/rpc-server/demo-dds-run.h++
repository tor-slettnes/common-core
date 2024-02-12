// -*- c++ -*-
//==============================================================================
/// @file demo-dds-server.h++
/// @brief C++ demo - Launch DDS server implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-api.h++"

#include <memory>

namespace demo::dds
{
    void run_dds_service(
        std::shared_ptr<demo::API> api_provider,
        const std::string &identity,
        int domain_id);
}
