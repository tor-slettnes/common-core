/// -*- c++ -*-
//==============================================================================
/// @file dds-responder.h++
/// @brief An server-issued response in a DDS environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "dds-endpoint.h++"
#include "chrono/date-time.h++"
#include "status/exceptions.h++"

#include "common-types.hpp"  // Generated from `common-types.idl'

#include <rti/request/SimpleReplier.hpp>

namespace cc::dds
{
    template <class RequestType = CC::Common::Empty,
              class ResponseType = CC::Common::Empty>
    class Responder : public Endpoint,
                      public rti::request::SimpleReplier<RequestType, ResponseType>
    {
        using This = Responder;
        using BaseResponder = rti::request::SimpleReplier<RequestType, ResponseType>;

    public:
        Responder(const std::string &request_id,
                  int domain_id,
                  std::function<ResponseType(RequestType)> &&handler)
            : Endpoint("responder", request_id, domain_id),
              BaseResponder(this->replier_params(), std::move(handler))
        {
        }

    protected:
        inline rti::request::ReplierParams replier_params() const
        {
            rti::request::ReplierParams params(this->get_participant());
            params.service_name(this->instance_name());
            return params;
        }
    };

}  // namespace cc::dds
