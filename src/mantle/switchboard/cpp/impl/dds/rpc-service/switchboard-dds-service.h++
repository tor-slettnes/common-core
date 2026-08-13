// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-service.h++
/// @brief Base class for Switchboard servers (standalone or relay)
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switchboard-dds-requesthandler.h++"
#include "switchboard-dds-publisher.h++"
#include "dds-service.h++"

namespace cc::platform::switchboard::dds
{
    //==========================================================================
    // @class Service
    // @brief Handle requests from Switchboard clients

    class Service : public cc::dds::Service<CC::Switchboard::SwitchboardService>
    {
        using This = Service;
        using Super = cc::dds::Service<CC::Switchboard::SwitchboardService>;

    public:
        Service(const ::dds::rpc::Server &server,
                int domain_id,
                const std::shared_ptr<Provider> &provider);

        void initialize() override;
        void deinitialize() override;
    };

}  // namespace cc::platform::switchboard::dds
