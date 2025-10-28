/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-error.c++
/// @brief Report and process ZMQ invocation status
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-protobuf-error.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

#include "chrono/date-time.h++"
#include "status/exceptions.h++"

namespace core::zmq
{
    ProtoBufError::ProtoBufError(const cc::protobuf::request_reply::StatusCode &code,
                                 const core::status::Error &error)
        : Super(error),
          status_code_(code)
    {
    }

    bool ProtoBufError::equivalent(const Event &other) const noexcept
    {
        if (auto *that = dynamic_cast<const ProtoBufError*>(&other))
        {
            return Super::equivalent(other) &&
                (this->status_code() == that->status_code());
        }
        else
        {
            return false;
        }
    }

    cc::protobuf::request_reply::StatusCode ProtoBufError::status_code() const
    {
        return this->status_code_;
    }

    status::Domain ProtoBufError::domain() const noexcept
    {
        return status::Domain::APPLICATION;
    }

    std::string ProtoBufError::class_name() const noexcept
    {
        return "Error";
    }

    std::exception_ptr ProtoBufError::as_application_error() const
    {
        switch (this->status_code())
        {
        case cc::protobuf::request_reply::StatusCode::STATUS_OK:
        case cc::protobuf::request_reply::StatusCode::STATUS_ACCEPTED:
            return {};

        case cc::protobuf::request_reply::StatusCode::STATUS_INVALID:
            return std::make_exception_ptr<exception::InvalidArgument>(*this);

        case cc::protobuf::request_reply::StatusCode::STATUS_CANCELLED:
            return std::make_exception_ptr<exception::Cancelled>(*this);

        case cc::protobuf::request_reply::StatusCode::STATUS_FAILED:
            return std::make_exception_ptr<exception::RuntimeError>(*this);

        default:
            return std::make_exception_ptr<exception::UnknownError>(*this);
        }
    }

    std::vector<std::string> ProtoBufError::field_names() const noexcept
    {
        return this->status_fields();
    }

    std::vector<std::string> ProtoBufError::status_fields() noexcept
    {
        std::vector<std::string> fields = Event::event_fields();
        fields.insert(fields.begin(), STATUS_FIELD_CODE);
        return fields;
    }

    types::Value ProtoBufError::get_field_as_value(const std::string &field_name) const
    {
        if (field_name == STATUS_FIELD_CODE)
        {
            return cc::protobuf::request_reply::StatusCode_Name(this->status_code());
        }
        else
        {
            return Super::get_field_as_value(field_name);
        }
    }

}  // namespace core::zmq
