// -*- c++ -*-
//==============================================================================
/// @file grpc-status.c++
/// @brief gRPC status with detailed error data
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-status.h++"
#include "protobuf-event-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"
#include "status/event.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include "grpc-status-names.gen"  // Generated by `create_grpcstatus.py`

namespace core::grpc
{

    //==========================================================================
    // \class Status

    Status::Status()
    {
    }

    Status::Status(const ::grpc::Status &status)
        : ::grpc::Status(status.error_code(),
                         status.error_message(),
                         status.error_details())
    {
        ::core::io::proto::decode(this->details(), this);
    }

    Status::Status(const Status &status)
        : Status(status.status_code(), status)
    {
    }

    Status::Status(const Event &event)
        : Status(Status::code_from_event(event), event)
    {
    }

    Status::Status(const cc::status::Event &details)
        : Event(
              details.text(),
              ::core::io::proto::decoded<status::Domain>(details.domain()),
              details.origin(),
              details.code(),
              details.symbol(),
              ::core::io::proto::decoded<status::Level>(details.level()),
              ::core::io::proto::decoded<status::Flow>(details.flow()),
              ::core::io::proto::decoded<dt::TimePoint>(details.timestamp()),
              ::core::io::proto::decoded<types::KeyValueMap>(details.attributes())),
          ::grpc::Status(
              Status::code_from_event(*this),
              details.text(),
              details.SerializeAsString())
    {
    }

    Status::Status(::grpc::StatusCode status_code,
                   const std::string &text,
                   const cc::status::Event &details)
        : Event(
              text,
              ::core::io::proto::decoded<status::Domain>(details.domain()),
              details.origin(),
              details.code(),
              details.symbol(),
              ::core::io::proto::decoded<status::Level>(details.level()),
              ::core::io::proto::decoded<status::Flow>(details.flow()),
              ::core::io::proto::decoded<dt::TimePoint>(details.timestamp()),
              ::core::io::proto::decoded<types::KeyValueMap>(details.attributes())),
          ::grpc::Status(
              status_code,
              text,
              details.SerializeAsString())
    {
    }

    Status::Status(::grpc::StatusCode status_code,
                   const Event &event)
        : Status(status_code,
                 event.text(),
                 event.domain(),
                 event.origin(),
                 event.code(),
                 event.symbol(),
                 event.level(),
                 event.flow(),
                 event.timepoint(),
                 event.attributes())
    {
    }

    Status::Status(::grpc::StatusCode status_code,
                   const std::string &text,
                   status::Domain domain,
                   const std::string &origin,
                   Code code,
                   const Symbol &symbol,
                   status::Level level,
                   status::Flow flow,
                   const dt::TimePoint &timepoint,
                   const types::KeyValueMap &attributes)
        : Event(text, domain, origin, code, symbol, level, flow, timepoint, attributes),
          ::grpc::Status(
              status_code,
              text,
              ::core::io::proto::encoded<cc::status::Event>(
                  *static_cast<Event *>(this))
                  .SerializeAsString())
    {
    }

    ::grpc::StatusCode Status::status_code() const noexcept
    {
        return this->error_code();
    }

    std::string Status::status_code_name() const noexcept
    {
        return Status::status_code_name(this->status_code());
    }

    std::string Status::status_code_name(::grpc::StatusCode status_code) noexcept
    {
        try
        {
            return grpc_status_names.at(status_code);
        }
        catch (const std::out_of_range &e)
        {
            return std::to_string(status_code);
        }
    }

    std::string Status::text() const noexcept
    {
        return this->error_message();
    }

    cc::status::Event Status::details() const noexcept
    {
        cc::status::Event msg;
        msg.ParseFromString(this->error_details());
        return msg;
    };

    void Status::to_stream(std::ostream &stream) const
    {
        types::TaggedValueList fields;
        this->populate_fields(&fields);

        stream << this->class_name()
               << "("
               << this->status_code_name();

        if (!this->text().empty())
        {
            stream << ", " << std::quoted(this->text());
        }

        if (!fields.empty())
        {
            fields.to_stream(stream,  // stream
                             ", ",    // prefix
                             ", ",    // infix
                             "");     // postfix
        }

        stream << ")";
    }

    std::string Status::class_name() const noexcept
    {
        return TYPE_NAME_BASE(Status);
    }

    ::grpc::StatusCode Status::code_from_event(const status::Event &event) noexcept
    {
        switch (event.domain())
        {
        case status::Domain::NONE:
            return event.empty()
                       ? ::grpc::StatusCode::OK
                       : ::grpc::StatusCode::UNKNOWN;

        case status::Domain::APPLICATION:
        case status::Domain::SYSTEM:
            return Status::code_from_errno(event.code());

        case status::Domain::PERIPHERAL:
            return ::grpc::StatusCode::ABORTED;

        default:
            return ::grpc::StatusCode::UNKNOWN;
        }
    }

    ::grpc::StatusCode Status::code_from_errno(int err) noexcept
    {
        switch (static_cast<std::errc>(err))
        {
        case std::errc::operation_canceled:
            return ::grpc::StatusCode::CANCELLED;

        case std::errc::timed_out:
            return ::grpc::StatusCode::DEADLINE_EXCEEDED;

        case std::errc::address_in_use:
        case std::errc::file_exists:
        case std::errc::already_connected:
            return ::grpc::StatusCode::ALREADY_EXISTS;

        case std::errc::operation_not_supported:
            return ::grpc::StatusCode::FAILED_PRECONDITION;

        case std::errc::no_such_file_or_directory:
        case std::errc::no_such_device_or_address:
        case std::errc::no_such_device:
        case std::errc::no_such_process:
            return ::grpc::StatusCode::NOT_FOUND;

        case std::errc::permission_denied:
        case std::errc::operation_not_permitted:
            return ::grpc::StatusCode::PERMISSION_DENIED;

        case std::errc::result_out_of_range:
            return ::grpc::StatusCode::OUT_OF_RANGE;

        case std::errc::file_too_large:
        case std::errc::no_space_on_device:
            return ::grpc::StatusCode::RESOURCE_EXHAUSTED;

        case std::errc::invalid_argument:
        case std::errc::filename_too_long:
        case std::errc::argument_out_of_domain:
        case std::errc::argument_list_too_long:
            return ::grpc::StatusCode::INVALID_ARGUMENT;

        case std::errc::device_or_resource_busy:
        case std::errc::operation_in_progress:
        case std::errc::text_file_busy:
        case std::errc::interrupted:
            return ::grpc::StatusCode::ABORTED;

        case std::errc::io_error:
        case std::errc::resource_unavailable_try_again:
            return ::grpc::StatusCode::UNAVAILABLE;

        default:
            return ::grpc::StatusCode::UNKNOWN;
        }
    }

    void Status::throw_if_error() const
    {
        if (!this->ok())
        {
            Event::throw_if_error();
        }
    }

    std::exception_ptr Status::as_exception_ptr() const
    {
        if ((this->domain() == status::Domain::NONE) && !this->ok())
        {
            return this->as_application_error();
        }
        else
        {
            return Event::as_exception_ptr();
        }
    }

    std::exception_ptr Status::as_application_error() const
    {
        switch (this->status_code())
        {
        case ::grpc::StatusCode::CANCELLED:
            return std::make_exception_ptr<exception::Cancelled>(*this);

        case ::grpc::StatusCode::DEADLINE_EXCEEDED:
            return std::make_exception_ptr<exception::Timeout>(*this);

        case ::grpc::StatusCode::INVALID_ARGUMENT:
            return std::make_exception_ptr<exception::InvalidArgument>(*this);

        case ::grpc::StatusCode::OUT_OF_RANGE:
            return std::make_exception_ptr<exception::OutOfRange>(*this);

        case ::grpc::StatusCode::FAILED_PRECONDITION:
            return std::make_exception_ptr<exception::FailedPrecondition>(*this);

        case ::grpc::StatusCode::NOT_FOUND:
            return std::make_exception_ptr<exception::NotFound>(*this);

        case ::grpc::StatusCode::ALREADY_EXISTS:
            return std::make_exception_ptr<exception::Duplicate>(*this);

        case ::grpc::StatusCode::PERMISSION_DENIED:
            return std::make_exception_ptr<exception::PermissionDenied>(*this);

        case ::grpc::StatusCode::RESOURCE_EXHAUSTED:
            return std::make_exception_ptr<exception::ResourceExhausted>(*this);

        case ::grpc::StatusCode::ABORTED:
            return std::make_exception_ptr<exception::RuntimeError>(*this);

        case ::grpc::StatusCode::UNAVAILABLE:
            return std::make_exception_ptr<exception::Unavailable>(*this);

        default:
            return std::make_exception_ptr<exception::UnknownError>(*this);
        }
    }

}  // namespace core::grpc
