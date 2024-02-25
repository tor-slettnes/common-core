/// -*- c++ -*-
//==============================================================================
/// @file grpc-status.h++
/// @brief gRPC status with detailed error data
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "status/event.h++"
#include "chrono/date-time.h++"
#include "types/value.h++"
#include "string/misc.h++"

#include "event_types.pb.h"

// System headers
#include <grpc++/grpc++.h>

namespace shared::grpc
{
    //==========================================================================
    /// @class Status
    /// @brief Specialization of ::grpc::Status with embedded Details
    ///     and in turn abstract base for source-specific exception types below.

    class Status : public status::Event,
                   public ::grpc::Status
    {
    public:
        /// @brief
        ///     Empty constructor
        Status();

        /// @brief
        ///     Constructor from an existing ::grpc::Status instance.
        /// @param[in] status
        ///     Existing instance.
        /// @note
        ///     This constructor deserializes the error_details() string of the
        ///     existing instance, under the assumption that it represents a
        ///     Status payload.
        Status(const ::grpc::Status &status);

        /// @brief
        ///     Constructor from another Status instance
        /// @param[in] status
        ///     Status instance
        Status(const Status &status);

        /// @brief
        ///     Constructor from an Error instance
        /// @param[in] event
        ///     Event instance
        Status(const Event &event);

        /// @brief
        ///     Constructor with an existing Details input
        /// @param[in] details
        ///     Already-constructed CC::Status::Details ProtoBuf container
        Status(const CC::Status::Details &details);

        /// @brief
        ///     Constructor with an existing Details input
        /// @param[in] status_code
        ///     gRPC error code, see /usr/include/grpcpp/impl/codegen/status_code_enum.h
        /// @param[in] text
        ///     Human readable text.
        /// @param[in] details
        ///     Already-constructed CC::Status::Details ProtoBuf container
        Status(::grpc::StatusCode status_code,
               const std::string &text,
               const CC::Status::Details &details);

        /// @brief
        ///     Constructor from an Error instance with explicit status code
        /// @param[in] status_code
        ///     gRPC Status
        /// @param[in] event
        ///     Event instance
        Status(::grpc::StatusCode status_code, const Event &event);

        /// @brief
        ///     Constructor from individual attributes
        /// @param[in] status_code
        ///     gRPC error code, see /usr/include/grpcpp/impl/codegen/status_code_enum.h
        /// @param[in] text
        ///     Human readable text.
        /// @param[in] domain
        ///     Source domain: SERVICE, SYSTEM, SIGNAL, PERIPHERAL
        /// @param[in] origin
        ///     Specific source, e.g OS name, service name, device name
        /// @param[in] code
        ///     Numeric error code within specified domain if available
        /// @param[in] symbol
        ///     Symbolic error ID within specified domain if available
        /// @param[in] level
        ///     Severity level
        /// @param[in] flow
        ///     Execution flow
        /// @param[in] timepoint
        ///     Time of occurence.
        /// @param[in] attributes
        ///     Additional information specific to the error
        Status(::grpc::StatusCode status_code,
               const std::string &text,
               status::Domain domain = status::Domain::APPLICATION,
               const std::string &origin = "",
               Code code = 0,
               const Symbol &symbol = "",
               status::Level level = status::Level::NONE,
               status::Flow flow = status::Flow::NONE,
               const dt::TimePoint &timepoint = dt::Clock::now(),
               const types::KeyValueMap &attributes = {});

        /// @brief
        ///     Get the gRPC status code that was provided in the constructor.
        /// @return
        ///     ::grpc::StatusCode enumeration
        /// @note
        ///     This is simply a wrapper for ::grpc::Status::error_code().
        ::grpc::StatusCode status_code() const noexcept;
        std::string status_code_name() const noexcept;
        static std::string status_code_name(::grpc::StatusCode status_code) noexcept;

        /// @brief
        ///     Get the explanatory text that was provided in the constructor.
        /// @return
        ///     Human readable text
        /// @note
        ///     This is simply a wrapper for ::grpc::Status::error_message().
        std::string text() const noexcept override;

        /// @brief
        ///     Get the details payload of this status instance.
        /// @return
        ///     Details as a CC::Status::Details instance.
        CC::Status::Details details() const noexcept;

        /// @brief
        ///     Throw an appropriate error if status is not OK
        void throw_if_error() const override;

        /// @brief
        ///     Throw as an appropriate error based on domain, code, and id
        std::exception_ptr as_exception_ptr() const override;

    protected:
        std::string class_name() const noexcept override;
        void to_stream(std::ostream &stream) const override;
        std::exception_ptr as_application_error() const override;

    private:
        static ::grpc::StatusCode code_from_event(const status::Event &event) noexcept;
        static ::grpc::StatusCode code_from_errno(int code) noexcept;
    };
}  // namespace shared::grpc
