/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-status.c++
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
    ProtoBufError::ProtoBufError(const cc::rr::StatusCode &code,
                                 const core::status::Event &event)
        : Event(event),
          status_code_(code)
    {
    }

    cc::rr::StatusCode ProtoBufError::status_code() const
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

    void ProtoBufError::populate_fields(types::TaggedValueList *values) const noexcept
    {
        values->emplace_back(STATUS_FIELD_CODE, this->status_code());
        Event::populate_fields(values);
    }

    std::exception_ptr ProtoBufError::as_application_error() const
    {
        switch (this->status_code())
        {
        case cc::rr::StatusCode::STATUS_OK:
        case cc::rr::StatusCode::STATUS_ACCEPTED:
            return {};

        case cc::rr::StatusCode::STATUS_INVALID:
            return std::make_exception_ptr<exception::InvalidArgument>(*this);

        case cc::rr::StatusCode::STATUS_CANCELLED:
            return std::make_exception_ptr<exception::Cancelled>(*this);

        case cc::rr::StatusCode::STATUS_FAILED:
            return std::make_exception_ptr<exception::RuntimeError>(*this);

        default:
            return std::make_exception_ptr<exception::UnknownError>(*this);
        }
    }

}  // namespace core::zmq
