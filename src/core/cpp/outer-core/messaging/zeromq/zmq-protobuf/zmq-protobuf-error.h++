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
        using This = ProtoBufError;
        using Super = status::Event;

    public:
        using status::Event::Event;

        ProtoBufError(const cc::rr::StatusCode &code,
                      const core::status::Event &event);

        cc::rr::StatusCode status_code() const;
        status::Domain domain() const noexcept override;
        std::exception_ptr as_application_error() const override;

        std::string class_name() const noexcept override;

        static std::vector<std::string> status_fields() noexcept;
        std::vector<std::string> field_names() const noexcept override;
        types::Value get_field_as_value(const std::string &field_name) const override;

    private:
        cc::rr::StatusCode status_code_;
    };
}  // namespace core::zmq
