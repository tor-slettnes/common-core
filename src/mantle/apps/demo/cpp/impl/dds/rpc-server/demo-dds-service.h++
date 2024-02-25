// -*- c++ -*-
//==============================================================================
/// @file demo-dds-service.h++
/// @brief Demo DDS service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-dds-requesthandler.h++"
#include "dds-service.h++"

namespace demo::dds
{
    //==========================================================================
    // @class Service
    // @brief Handle requests from DemoService clients

    class Service : public shared::dds::Service<CC::Demo::DemoServiceService>
    {
        using Super = shared::dds::Service<CC::Demo::DemoServiceService>;

    public:
        Service(const ::dds::rpc::Server &server,
                int domain_id,
                const std::shared_ptr<API> &api);
    };

}  // namespace demo::dds
