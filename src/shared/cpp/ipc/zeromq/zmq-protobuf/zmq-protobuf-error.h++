/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-status.h++
/// @brief Report and process ZMQ invocation status
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "status/event.h++"

#include "request_reply.pb.h"

namespace cc::zmq
{
    constexpr auto STATUS_FIELD_CODE = "status";

    class ProtoBufError : public status::Event
    {
    public:
        using status::Event::Event;

        ProtoBufError(const CC::RR::StatusCode &code,
                      const cc::status::Event &event);

        CC::RR::StatusCode status_code() const;
        status::Domain domain() const noexcept override;
        std::exception_ptr as_application_error() const override;

        std::string class_name() const noexcept override;
        void populate_fields(types::TaggedValueList *values) const noexcept override;

    private:
        CC::RR::StatusCode status_code_;
    };
}  // namespace cc::zmq
