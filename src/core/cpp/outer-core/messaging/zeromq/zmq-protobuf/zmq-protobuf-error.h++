/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-error.h++
/// @brief Report and process ZMQ invocation status
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "status/event.h++"

#include "request_reply.pb.h"

namespace core::zmq
{
    constexpr auto STATUS_FIELD_CODE = "status";

    class ProtoBufError : public status::Event
    {
    public:
        using status::Event::Event;

        ProtoBufError(const cc::rr::StatusCode &code,
                      const core::status::Event &event);

        cc::rr::StatusCode status_code() const;
        status::Domain domain() const noexcept override;
        std::exception_ptr as_application_error() const override;

        std::string class_name() const noexcept override;
        void populate_fields(types::PartsList *parts) const noexcept override;

    private:
        cc::rr::StatusCode status_code_;
    };
}  // namespace core::zmq
