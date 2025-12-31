// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-service.c++
/// @brief Base class for Switchboard servers (standalone or relay)
/// @author Tor Slettnes
//==============================================================================

#include "switchboard-dds-service.h++"

namespace switchboard::dds
{
    Service::Service(const ::dds::rpc::Server &server,
                     int domain_id,
                     const std::shared_ptr<Provider> &api_provider)
        : Super(server,
                CC::Switchboard::SERVICE_INTERFACE_ID,
                domain_id,
                RequestHandler::create_shared(api_provider))
    {
    }

    void Service::initialize()
    {
        switchboard::start_event_capture();
        Super::initialize();
    }

    void Service::deinitialize()
    {
        Super::deinitialize();
        switchboard::stop_event_capture();
    }
}  // namespace switchboard
