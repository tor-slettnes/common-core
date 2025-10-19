/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-error.h++
/// @brief Report and process ZMQ invocation status
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "status/error.h++"

#include "cc/protobuf/core/request_reply.pb.h"

namespace core::zmq
{
    constexpr auto STATUS_FIELD_CODE = "status";

    class ProtoBufError : public status::Error
    {
        using This = ProtoBufError;
        using Super = status::Error;

    public:
        using status::Error::Error;

        ProtoBufError(const cc::rr::StatusCode &code,
                      const core::status::Error &error);

        bool equivalent(const Event &other) const noexcept override;
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
