/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-error.h++
/// @brief Report and process ZMQ invocation status
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "status/error.h++"

#include "cc/protobuf/request_reply/request_reply.pb.h"

namespace cc::zmq
{
    constexpr auto STATUS_FIELD_CODE = "status";

    class ProtoBufError : public core::status::Error
    {
        using This = ProtoBufError;
        using Super = core::status::Error;

    public:
        using core::status::Error::Error;

        ProtoBufError(const cc::protobuf::request_reply::StatusCode& code,
                      const core::status::Error& error);

        bool equivalent(const Event& other) const noexcept override;
        cc::protobuf::request_reply::StatusCode status_code() const;
        core::status::Domain domain() const noexcept override;
        std::exception_ptr as_application_error() const override;

        std::string class_name() const noexcept override;

        static std::vector<std::string> status_fields() noexcept;
        std::vector<std::string> field_names() const noexcept override;
        core::types::Value get_field_as_value(const std::string& field_name) const override;

    private:
        cc::protobuf::request_reply::StatusCode status_code_;
    };
}  // namespace cc::zmq
