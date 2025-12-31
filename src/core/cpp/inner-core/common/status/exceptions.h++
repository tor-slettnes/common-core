/// -*- c++ -*-
//==============================================================================
/// @file exceptions.h++
/// @brief Generic exception types, derived from `Event`
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "exception.h++"
#include "event.h++"
#include "platform/symbols.h++"
#include "platform/path.h++"
#include "platform/process.h++"

#include <stdexcept>

namespace core::exception
{
    //==========================================================================
    /// @class CustomException
    /// @brief `Event` wrapper for local errors derived on std::exception

    template <class Exc, class Class>
    class CustomException : public Exception<Exc>
    {
    protected:
        // These symbols are defined for use in derived classes so
        // `This` refers to the derived class.
        using This = Class;
        using Super = CustomException<Exc, Class>;

    public:
        inline CustomException(const status::Error &event)
            : Exception<Exc>(
                  event,
                  TYPE_NAME_BASE(Class))
        {
        }

        inline CustomException(std::errc code,
                               const status::Error::Symbol &symbol,
                               const std::string &text,
                               const types::KeyValueMap &attributes = {},
                               status::Level level = status::Level::ERROR)
            : Exception<Exc>(
                  {
                      text,                                    // text
                      status::Domain::APPLICATION,             // domain
                      platform::path->exec_name(),             // origin
                      static_cast<status::Error::Code>(code),  // code
                      symbol,                                  // symbol
                      level,                                   // level
                      {},                                      // timepoint
                      attributes,                              // attributes
                  },
                  TYPE_NAME_BASE(Class))  // class_name
        {
        }
    };

    //==========================================================================
    /// @class Cancelled
    /// @brief Indicates that an operation was cancelled

    class Cancelled : public CustomException<std::runtime_error, Cancelled>
    {
    public:
        using Super::Super;

        Cancelled(const std::string &msg = "Cancelled",
                  const std::string &operation = "");
    };

    //==========================================================================
    /// @class Timeout
    /// @brief Indicates that an operation timed out, along with the timeout value.

    class Timeout : public CustomException<std::runtime_error, Timeout>
    {
    public:
        using Super::Super;

        Timeout(std::string msg, dt::Duration timeout = dt::Duration::zero());
        Timeout(dt::Duration timeout = dt::Duration::zero());
    };

    //==========================================================================
    /// @class InvalidArgument
    /// @brief Indicates an invalid argument value.

    class InvalidArgument : public CustomException<std::invalid_argument, InvalidArgument>
    {
    public:
        using Super::Super;

        InvalidArgument(const std::string &msg = "Invalid Argument",
                        const types::Value &argument = {});
    };

    /// Indicates not enough arguments were provided
    class MissingArgument : public CustomException<std::invalid_argument, MissingArgument>
    {
    public:
        using Super::Super;

        MissingArgument(const std::string &msg = "Missing Argument",
                        uint provided = 0,
                        uint expected = 1);
    };

    /// Indicates too many arguments were provided
    class ExtraneousArgument : public CustomException<std::length_error, ExtraneousArgument>
    {
    public:
        using Super::Super;

        ExtraneousArgument(const std::string &msg = "Extraneous Argument",
                           uint provided = 0,
                           uint expected = 0);
    };

    //==========================================================================
    /// @class OutOfRange
    /// @brief Indicates that a out-of-bounds index or missing key

    class OutOfRange : public CustomException<std::out_of_range, OutOfRange>
    {
    public:
        using Super::Super;

        OutOfRange(const std::string &msg = "Out of range", const types::Value &item = {});
    };

    //==========================================================================
    /// @class NotFound
    /// @brief Indicates that an item was not found (e.g. in a lookup table)

    class NotFound : public CustomException<std::out_of_range, NotFound>
    {
    public:
        using Super::Super;

        NotFound(const std::string &msg = "Item not found", const types::Value &item = {});
    };

    //==========================================================================
    /// @class Duplicate
    /// @brief Indicates that an item was duplicated

    class Duplicate : public CustomException<std::out_of_range, Duplicate>
    {
    public:
        using Super::Super;

        Duplicate(const std::string &msg = "Duplicate item",
                  const types::Value &item = {});
    };

    //==========================================================================
    /// @class FailedPrecondition
    /// @brief Indicates that an operation could not proceed

    class FailedPrecondition : public CustomException<std::runtime_error, FailedPrecondition>
    {
    public:
        using Super::Super;

        FailedPrecondition(const std::string &msg = "Failed Precondition",
                           const types::KeyValueMap &attributes = {});
    };

    //==========================================================================
    /// @class FailedPostcondition
    /// @brief Indicates that an operation could not proceed

    class FailedPostcondition : public CustomException<std::runtime_error, FailedPostcondition>
    {
    public:
        using Super::Super;

        FailedPostcondition(const std::string &msg = "Failed Postcondition",
                            const types::KeyValueMap &attributes = {});
    };

    //==========================================================================
    /// @class ResourceExhausted
    /// @brief Indicates that an operation failed due to lack of system resources

    class ResourceExhausted : public CustomException<std::runtime_error, ResourceExhausted>
    {
    public:
        using Super::Super;

        ResourceExhausted(const std::string &msg = "Resource exhausted",
                          const std::string &resource = "");
    };

    //==========================================================================
    /// @class Unavailable
    /// @brief Indicates that an resource is currenly unavailable

    class Unavailable : public CustomException<std::runtime_error, Unavailable>
    {
    public:
        using Super::Super;

        Unavailable(const std::string &msg = "Resource unavailable",
                    const std::string &resource = "");
    };

    //==========================================================================
    /// @class RuntimeError
    /// @brief Indicates that an operation failed

    class RuntimeError : public CustomException<std::runtime_error, RuntimeError>
    {
    public:
        using Super::Super;

        RuntimeError(const std::string &msg = "Runtime Error",
                     const types::KeyValueMap &attributes = {});
    };

    //==========================================================================
    /// @class UnknownError
    /// @brief Represents an unknown exception

    class UnknownError : public CustomException<std::runtime_error, UnknownError>
    {
    public:
        using Super::Super;

        UnknownError(const std::string &msg = "Unknown Error",
                     const types::KeyValueMap &attributes = {});
    };

    //==========================================================================
    /// @class InvocationError
    /// @brief Error created from a process invocation

    class InvocationError : public Exception<std::invalid_argument>
    {
        using This = InvocationError;
        using Super = Exception<std::invalid_argument>;

    public:
        InvocationError(const status::Error &event);

        InvocationError(const std::string &source,
                        int exit_code,
                        const std::string &symbol,
                        const std::string &text);

        InvocationError(const std::string &source,
                        const platform::ExitStatus::ptr &exit_status);

        InvocationError(const std::string &source,
                        const platform::InvocationResult &result);
    };

    //==========================================================================
    /// @class UnsupportedError
    /// @brief Operation is not supported

    class UnsupportedError : public CustomException<std::logic_error, UnsupportedError>
    {
        using This = UnsupportedError;
        using Super = CustomException<std::logic_error, UnsupportedError>;

    public:
        using Super::Super;

        UnsupportedError(const std::string &msg = "Unsupported operation",
                         const types::KeyValueMap &attributes = {});
    };

    //==========================================================================
    /// @class SystemError
    /// @brief Error created from a `std::system_error` instance

    class SystemError : public Exception<std::system_error>
    {
        using This = SystemError;
        using Super = Exception<std::system_error>;

    public:
        SystemError(const status::Error &event);
        SystemError(const std::system_error &e,
                    const std::optional<std::string> &preamble = {});

        explicit SystemError(int errcode, const std::string &what);
        explicit SystemError(int errcode = errno);
        explicit SystemError(const std::string &preamble,
                             int errcode,
                             const std::string &what);

        explicit SystemError(const std::string &preamble, int errcode = errno);
        explicit SystemError(const std::error_category &category,
                             int errorcode,
                             const std::string &what);
    };

    //==========================================================================
    /// @class PermissionDenied
    /// @brief Indicates that an item could not be accessed due to missing permissions

    class PermissionDenied : public Exception<std::system_error>
    {
        using This = PermissionDenied;
        using Super = Exception<std::system_error>;

    public:
        PermissionDenied(const std::string &msg = "Permission denied",
                         const std::string &operation = "");

        PermissionDenied(const status::Error &event);
    };

    //==========================================================================
    /// @class FilesystemError
    /// @brief Error created from a `std::filesystem_error` instance

    class FilesystemError : public Exception<fs::filesystem_error>
    {
        using This = FilesystemError;
        using Super = Exception<fs::filesystem_error>;

    public:
        FilesystemError(const status::Error &event);

        FilesystemError(const fs::filesystem_error &e);

        FilesystemError(const std::error_code &error_code,
                        const std::filesystem::path &path1,
                        const std::filesystem::path &path2,
                        const std::string &what);

        explicit FilesystemError(int errcode,
                                 const std::filesystem::path &path1,
                                 const std::filesystem::path &path2,
                                 const std::string &what);

        explicit FilesystemError(int errcode,
                                 const std::filesystem::path &path1,
                                 const std::filesystem::path &path2);

        explicit FilesystemError(int errcode,
                                 const std::filesystem::path &path1,
                                 const std::string &what);

        explicit FilesystemError(int errcode,
                                 const std::filesystem::path &path1);
    };

    //==========================================================================
    /// @class DeviceError
    /// @brief Indicates that a device error occured

    class DeviceError : public Exception<std::runtime_error>
    {
        using This = DeviceError;
        using Super = Exception<std::runtime_error>;

    public:
        using Super::Super;

        DeviceError(const std::string &text,
                    const std::string &device,
                    Code code,
                    const std::string &id,
                    status::Level level = status::Level::ERROR,
                    const dt::TimePoint &timepoint = {},
                    const types::KeyValueMap &attributes = {});
    };

    //==========================================================================
    /// @class ServiceError
    /// @brief Error received from a cascaded service

    class ServiceError : public Exception<std::runtime_error>
    {
        using This = ServiceError;
        using Super = Exception<std::runtime_error>;

    public:
        using Super::Super;

        ServiceError(const std::string &text,
                     const std::string &service,
                     Code code,
                     const std::string &id,
                     status::Level level = status::Level::ERROR,
                     const dt::TimePoint &timepoint = {},
                     const types::KeyValueMap &attributes = {});
    };

    //==========================================================================
    // Map various exceptions to appropriate Event objects

    status::Error::ptr map_to_error(const std::exception &e) noexcept;
    status::Error::ptr map_to_error(std::exception_ptr eptr) noexcept;

};  // namespace core::exception

namespace std
{
    /// Define output stream operator "<<" on std::exception and derivatives.
    std::ostream &operator<<(std::ostream &stream, const exception &e);
    std::ostream &operator<<(std::ostream &stream, exception_ptr eptr);
}  // namespace std
