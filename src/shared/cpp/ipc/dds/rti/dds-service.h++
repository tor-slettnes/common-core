/// -*- c++ -*-
//==============================================================================
/// @file dds-service.h++
/// @brief Mix-in base for DDS service implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "dds-channel.h++"
#include "types/create-shared.h++"
#include "platform/symbols.h++"
#include "logging/logging.h++"

#include <dds/rpc/ServiceParams.hpp>
#include <dds/rpc/Server.hpp>

namespace cc::dds
{
    //==========================================================================
    // DDS service Implementation template

    template <class ServiceType>
    class Service : public Channel,
                    public ServiceType
    {
    protected:
        template <class RequestHandler>
        Service(
            const ::dds::rpc::Server &server,
            const std::string &service_name,
            int domain_id,
            const std::shared_ptr<RequestHandler> &request_handler)
            : Channel(TYPE_NAME_FULL(ServiceType), service_name, domain_id),
              ServiceType(request_handler, server, this->service_params())
        {
            logf_debug("dds::Service<%s> constructor", TYPE_NAME_BASE(ServiceType));
        }

        virtual inline ::dds::rpc::ServiceParams service_params() const
        {
            ::dds::rpc::ServiceParams params(this->get_participant());
            params.service_name(this->channel_name());
            return params;
        }
    };

}  // namespace cc::dds
