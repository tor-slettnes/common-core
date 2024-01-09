// -*- c++ -*-
//==============================================================================
/// @file demo-dds-service.h++
/// @brief Demo DDS service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-dds-requesthandler.h++"
#include "dds-service.h++"

namespace cc::demo::dds
{
    //==========================================================================
    // @class Service
    // @brief Handle requests from DemoService clients

    class Service
        : public cc::dds::DDS_Service<CC::Demo::DemoServiceService>
    {
        using Super = cc::dds::DDS_Service<CC::Demo::DemoServiceService>;

    public:
        Service(const ::dds::rpc::Server &server,
                int domain_id,
                const std::shared_ptr<API> &api);

        void initialize() override;
        void deinitialize() override;
    };

}  // namespace cc::demo::dds
