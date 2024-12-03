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

#include "status.pb.h"

// System headers
#include <grpc++/grpc++.h>

namespace core::grpc
{
    constexpr auto STATUS_FIELD_CODE = "status_code";

    //==========================================================================
    /// @class Status
    /// @brief
    ///     Specialization of
    ///     [grpc::Status](https://grpc.github.io/grpc/cpp/classgrpc_1_1_status.html)
    ///     with embedded Details and in turn abstract base for source-specific
    ///     exception types below.

    class Status : public status::Event,
                   public ::grpc::Status
    {
        using This = Status;

    public:
        /// @brief
        ///     Empty constructor
        Status();

        /// @brief
        ///     Constructor from an existing
        ///     [grpc::Status](https://grpc.github.io/grpc/cpp/classgrpc_1_1_status.html)
        ///     instance.
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
        ///     Constructor from a `status::Event` instance
        /// @param[in] event
        ///     Event instance
        Status(const Event &event);

        /// @brief
        ///     Constructor with an existing Details input
        /// @param[in] details
        ///     Already-constructed cc::status::Event ProtoBuf container
        Status(const cc::status::Event &details);

        /// @brief
        ///     Constructor with an existing Details input
        /// @param[in] status_code
        ///     gRPC error code, see /usr/include/grpcpp/impl/codegen/status_code_enum.h
        /// @param[in] text
        ///     Human readable text.
        /// @param[in] details
        ///     Already-constructed cc::status::Event ProtoBuf container
        Status(::grpc::StatusCode status_code,
               const std::string &text,
               const cc::status::Event &details);

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
        ///     Source domain: SERVICE, SYSTEM, SIGNAL, DEVICE
        /// @param[in] origin
        ///     Specific source, e.g OS name, service name, device name
        /// @param[in] code
        ///     Numeric error code within specified domain if available
        /// @param[in] symbol
        ///     Symbolic error ID within specified domain if available
        /// @param[in] level
        ///     Severity level
        /// @param[in] timepoint
        ///     Time of occurence.
        /// @param[in] attributes
        ///     Additional information specific to the error
        /// @param[in] contract_id
        ///     Log contract, if any.  This informs the receiver of what fields
        ///     to expect as custom attributes.
        /// @param[in] host
        ///     Reporting host name.
        Status(::grpc::StatusCode status_code,
               const std::string &text,
               status::Domain domain = status::Domain::APPLICATION,
               const std::string &origin = "",
               Code code = 0,
               const Symbol &symbol = "",
               status::Level level = status::Level::NONE,
               const dt::TimePoint &timepoint = dt::Clock::now(),
               const types::KeyValueMap &attributes = {},
               const std::string &contract_id = {},
               const std::string &host = {});

        /// @brief
        ///     Get the gRPC status code that was provided in the constructor.
        /// @return
        ///     gRPC status code
        /// @note
        ///     This is simply a wrapper for grpc::Status::error_code().
        ::grpc::StatusCode status_code() const noexcept;
        std::string status_code_name() const noexcept;
        static std::string status_code_name(::grpc::StatusCode status_code) noexcept;

        /// @brief
        ///     Get the explanatory text that was provided in the constructor.
        /// @return
        ///     Human readable text
        /// @note
        ///     This is simply a wrapper for grpc::Status::error_message().
        std::string text() const noexcept override;

        /// @brief
        ///     Get the details payload of this status instance.
        /// @return
        ///     Details as a cc::status::Event instance.
        cc::status::Event details() const noexcept;

        /// @brief
        ///     Throw an appropriate error if status is not OK
        void throw_if_error() const override;

        /// @brief
        ///     Throw as an appropriate error based on domain, code, and id
        std::exception_ptr as_exception_ptr() const override;

    public:
        static std::vector<std::string> status_fields() noexcept;
        std::vector<std::string> field_names() const noexcept override;
        types::Value get_field_as_value(const std::string &field_name) const override;

    protected:
        std::string class_name() const noexcept override;
        std::exception_ptr as_application_error() const override;

    private:
        static ::grpc::StatusCode code_from_event(const status::Event &event) noexcept;
        static ::grpc::StatusCode code_from_errno(int code) noexcept;
    };
}  // namespace core::grpc
