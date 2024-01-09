/// -*- c++ -*-
//==============================================================================
/// @file dds-responder.h++
/// @brief An server-issued response in a DDS environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "dds-channel.h++"
#include "chrono/date-time.h++"
#include "status/exceptions.h++"

#include <rti/request/SimpleReplier.hpp>

namespace cc::dds
{
    using Empty = CC::Shared::Empty;

    template <class RequestType = Empty, class ResponseType = Empty>
    class DDS_Responder : public DDS_Channel,
                          public rti::request::SimpleReplier<RequestType, ResponseType>
    {
        using This = DDS_Responder;
        using Replier = rti::request::SimpleReplier<RequestType, ResponseType>;

    public:
        DDS_Responder(const std::string &request_id,
                      int domain_id,
                      std::function<ResponseType(RequestType)> &&handler)
            : DDS_Channel(TYPE_NAME_FULL(This), request_id, domain_id),
              Replier(this->replier_params(), std::move(handler))
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
