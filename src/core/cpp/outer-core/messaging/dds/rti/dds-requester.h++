/// -*- c++ -*-
//==============================================================================
/// @file dds-requester.h++
/// @brief An client-issued request in a DDS environment
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "dds-endpoint.h++"
#include "chrono/date-time.h++"
#include "status/exceptions.h++"

#include <rti/request/Requester.hpp>
//#include <dds/core/Duration.hpp>

namespace core::dds
{
    template <class RequestType, class ResponseType>
    class Requester : public Endpoint,
                      public rti::request::Requester<RequestType, ResponseType>
    {
        using This = Requester;
        using BaseRequester = rti::request::Requester<RequestType, ResponseType>;

    public:
        Requester(const std::string &request_id,
                  int domain_id,
                  const core::dt::Duration &default_timeout = std::chrono::seconds(10))
            : Endpoint("requester", request_id, domain_id),
              BaseRequester(this->requester_params()),
              default_timeout_(default_timeout)
        {
        }

    protected:
        inline rti::request::RequesterParams requester_params() const
        {
            rti::request::RequesterParams params(this->get_participant());
            params.service_name(this->instance_name());
            return params;
        }

    public:
        inline ResponseType send_receive(const RequestType &req = {})
        {
            return this->send_receive(req, this->default_timeout_);
        }

        inline ResponseType send_receive(const RequestType &req,
                                         const core::dt::Duration &max_wait)
        {
            rti::core::SampleIdentity request_id = this->send_request(req);
            return this->receive_response(request_id, max_wait);
        }

        ResponseType receive_response(const rti::core::SampleIdentity &request_id,
                                      const core::dt::Duration &max_wait)
        {
            if (this->wait_for_replies(1, max_wait, request_id))
            {
                for (const auto &reply : this->take_replies(request_id))
                {
                    if (reply.info().valid())
                    {
                        return reply.data();
                    }
                    else
                    {
                        throwf(std::runtime_error,
                               "Request %r received invalid response",
                               this->instance_name());
                    }
                }
            }
            throwf_args(core::exception::Timeout,
                        ("Request %r did not receive any responses", this->instance_name()),
                        max_wait);
        }

    private:
        core::dt::Duration default_timeout_;
    };

}  // namespace core::dds
