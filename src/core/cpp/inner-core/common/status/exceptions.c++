/// -*- c++ -*-
//==============================================================================
/// @file exceptions.c++
/// @brief Generic exception types, derived from `Event`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "exceptions.h++"
#include "platform/path.h++"
#include "platform/symbols.h++"
#include "types/create-shared.h++"

#include <exception>
#include <string.h>

namespace core::exception
{
    //==========================================================================
    // Cancelled

    Cancelled::Cancelled(const std::string &msg,
                         const std::string &operation)
        : Super(std::errc::operation_canceled,
                TYPE_NAME_BASE(This),
                msg,
                {{"operation", operation}})
    {
    }

    //==========================================================================
    // Timeout

    Timeout::Timeout(std::string msg, dt::Duration timeout)
        : Super(std::errc::timed_out,
                TYPE_NAME_BASE(This),
                msg,
                {{"timeout", timeout}})
    {
    }

    Timeout::Timeout(dt::Duration timeout)
        : Timeout(str::format("Timed out after %s seconds", timeout),
                  timeout)
    {
    }

    //==========================================================================
    // InvalidArgument

    InvalidArgument::InvalidArgument(const std::string &msg,
                                     const types::Value &argument)
        : Super(std::errc::invalid_argument,
                TYPE_NAME_BASE(This),
                msg,
                {{"argument", argument}})
    {
    }

    //==========================================================================
    // MissingArgument

    MissingArgument::MissingArgument(const std::string &msg,
                                     uint provided,
                                     uint expected)
        : Super(std::errc::invalid_argument,
                TYPE_NAME_BASE(This),
                msg,
                {{"provided", provided},
                 {"expected", expected}})
    {
    }

    //==========================================================================
    // ExtraneousArgument

    ExtraneousArgument::ExtraneousArgument(const std::string &msg,
                                           uint provided,
                                           uint expected)
        : Super(std::errc::argument_list_too_long,
                TYPE_NAME_BASE(This),
                msg,
                {{"provided", provided},
                 {"expected", expected}})
    {
    }

    //==========================================================================
    // OutOfRange

    OutOfRange::OutOfRange(const std::string &msg,
                           const types::Value &item)
        : Super(std::errc::result_out_of_range,
                TYPE_NAME_BASE(This),
                msg,
                {{"item", item}})
    {
    }

    //==========================================================================
    // FailedPrecondition

    FailedPrecondition::FailedPrecondition(const std::string &msg,
                                           const types::KeyValueMap &attributes)
        : Super(std::errc::operation_not_supported,
                TYPE_NAME_BASE(This),
                msg,
                attributes)
    {
    }

    //==========================================================================
    // FailedPostcondition

    FailedPostcondition::FailedPostcondition(const std::string &msg,
                                             const types::KeyValueMap &attributes)
        : Super(std::errc::interrupted,
                TYPE_NAME_BASE(This),
                msg,
                attributes)
    {
    }

    //==========================================================================
    // NotFound

    NotFound::NotFound(const std::string &msg,
                       const types::Value &item)
        : Super(std::errc::no_such_file_or_directory,
                TYPE_NAME_BASE(This),
                msg,
                {{"item", item}})
    {
    }

    //==========================================================================
    // Duplicate

    Duplicate::Duplicate(const std::string &msg,
                         const types::Value &item)
        : Super(std::errc::file_exists,
                TYPE_NAME_BASE(This),
                msg,
                {{"item", item}})
    {
    }

    //==========================================================================
    // ResourceExhausted

    ResourceExhausted::ResourceExhausted(const std::string &msg,
                                         const std::string &resource)
        : Super(std::errc::resource_unavailable_try_again,
                TYPE_NAME_BASE(This),
                msg,
                {{"resource", resource}})
    {
    }

    //==========================================================================
    // Unavailable

    Unavailable::Unavailable(const std::string &msg,
                             const std::string &resource)
        : Super(std::errc::resource_unavailable_try_again,
                TYPE_NAME_BASE(This),
                msg,
                {{"resource", resource}})
    {
    }

    //==========================================================================
    // RuntimeError

    RuntimeError::RuntimeError(const std::string &msg,
                               const types::KeyValueMap &attributes)
        : Super(std::errc::interrupted,
                TYPE_NAME_BASE(This),
                msg,
                attributes)
    {
    }

    //==========================================================================
    // UnknownError

    UnknownError::UnknownError(const std::string &msg,
                               const types::KeyValueMap &attributes)
        : Super(std::errc::interrupted,
                TYPE_NAME_BASE(This),
                msg,
                attributes)
    {
    }

    //==========================================================================
    // InvocationError

    InvocationError::InvocationError(const status::Error &event)
        : Super(event,
                std::invalid_argument(event.text()),
                TYPE_NAME_BASE(This))
    {
    }

    InvocationError::InvocationError(const std::string &source,
                                     int exit_code,
                                     const std::string &symbol,
                                     const std::string &text)
        : Super(status::Error(
                    text,
                    status::Domain::PROCESS,
                    source,
                    exit_code,
                    symbol,
                    status::Level::ERROR),
                std::invalid_argument(text),
                TYPE_NAME_BASE(This))
    {
    }

    InvocationError::InvocationError(const std::string &source,
                                     const platform::ExitStatus::ptr &exit_status)
        : InvocationError(
              source,
              exit_status->combined_code(),
              exit_status->symbol(),
              exit_status->text())
    {
    }

    InvocationError::InvocationError(const std::string &source,
                                     const platform::InvocationResult &result)
        : InvocationError(
              source,
              result.error_code(),
              result.error_symbol(),
              result.error_text())
    {
    }

    //==========================================================================
    // UnsupportedError

    UnsupportedError::UnsupportedError(const std::string &msg,
                                       const core::types::KeyValueMap &attributes)
        : Super(std::errc::function_not_supported,
                TYPE_NAME_BASE(This),
                msg,
                attributes)
    {
    }

    //==========================================================================
    // SystemError

    SystemError::SystemError(const status::Error &event)
        : Super(event,
                std::system_error(
                    std::error_code(event.code(), std::generic_category()),
                    event.text()),
                TYPE_NAME_BASE(This))
    {
    }

    SystemError::SystemError(const std::system_error &e,
                             const std::optional<std::string> &preamble)
        : Super(status::Error(
                    preamble ? preamble.value() + ": " + e.what()
                             : e.what(),         // text
                    status::Domain::SYSTEM,      // domain
                    e.code().category().name(),  // origin
                    e.code().value(),            // code
                    {},                          // id
                    status::Level::ERROR,       // level
                    {},                          // timepoint
                    {}),                         // attributes
                std::system_error(e),
                TYPE_NAME_BASE(This))
    {
    }

    SystemError::SystemError(int errcode,
                             const std::string &what)
        : SystemError(
              std::system_error(
                  std::error_code(errcode, std::generic_category()),
                  what))
    {
    }

    SystemError::SystemError(int errcode)
        : SystemError(errcode, strerror(errcode))
    {
    }

    SystemError::SystemError(const std::string &preamble,
                             int errcode,
                             const std::string &what)
        : SystemError(errcode, str::format("%s: %s", preamble, what))
    {
    }

    SystemError::SystemError(const std::string &preamble, int errcode)
        : SystemError(preamble, errcode, strerror(errcode))
    {
    }

    SystemError::SystemError(const std::error_category &category,
                             int errorcode,
                             const std::string &what)
        : SystemError(std::system_error(errorcode, category, what))
    {
    }

    //==========================================================================
    // PermissionDenied

    PermissionDenied::PermissionDenied(const status::Error &event)
        : Super(event,
                std::make_error_code(std::errc::permission_denied),
                TYPE_NAME_BASE(This))
    {
    }

    PermissionDenied::PermissionDenied(const std::string &msg,
                                       const std::string &operation)
        : Super(status::Error(
                    msg,                          // text
                    status::Domain::SYSTEM,       // domain
                    platform::path->exec_name(),  // origin
                    EPERM,                        // code
                    TYPE_NAME_BASE(This),         // symbol
                    status::Level::ERROR,        // level
                    {},                           // timepoint
                    {{"operation", operation}}),  // attributes
                std::make_error_code(std::errc::permission_denied),
                TYPE_NAME_BASE(This))
    {
    }

    //==========================================================================
    // FileSystemError

    FilesystemError::FilesystemError(const status::Error &event)
        : Super(event,
                fs::filesystem_error(
                    event.text(),
                    event.attribute("path1").as_string(),
                    event.attribute("path2").as_string(),
                    std::error_code(static_cast<int>(event.code()),
                                    std::system_category())),
                TYPE_NAME_BASE(This))
    {
    }

    FilesystemError::FilesystemError(const fs::filesystem_error &e)
        : Super(status::Error(
                    e.what(),                    // text
                    status::Domain::SYSTEM,      // domain
                    e.code().category().name(),  // origin
                    e.code().value(),            // code
                    TYPE_NAME_BASE(This),        // id
                    status::Level::ERROR,       // level
                    {},                          // timepoint
                    {
                        {"path1", types::Value(e.path1().string())},
                        {"path2", types::Value(e.path2().string())},
                    }),
                fs::filesystem_error(e),
                TYPE_NAME_BASE(This))
    {
    }

    FilesystemError::FilesystemError(const std::error_code &error_code,
                                     const std::filesystem::path &path1,
                                     const std::filesystem::path &path2,
                                     const std::string &what)
        : FilesystemError(fs::filesystem_error(
              what,
              path1,
              path2,
              error_code))
    {
    }

    FilesystemError::FilesystemError(int errcode,
                                     const std::filesystem::path &path1,
                                     const std::filesystem::path &path2,
                                     const std::string &what)
        : FilesystemError(
              std::error_code(static_cast<int>(errcode), std::system_category()),
              path1,
              path2,
              what)
    {
    }

    FilesystemError::FilesystemError(int errcode,
                                     const std::filesystem::path &path1,
                                     const std::filesystem::path &path2)
        : FilesystemError(errcode, path1, path2, strerror(errcode))
    {
    }

    FilesystemError::FilesystemError(int errcode,
                                     const std::filesystem::path &path1,
                                     const std::string &what)
        : FilesystemError(errcode, path1, {}, what)
    {
    }

    FilesystemError::FilesystemError(int errcode,
                                     const std::filesystem::path &path1)
        : FilesystemError(errcode, path1, {}, strerror(errcode))
    {
    }

    //==========================================================================
    // DeviceError

    DeviceError::DeviceError(const std::string &text,
                             const std::string &device,
                             Code code,
                             const std::string &id,
                             status::Level level,
                             const dt::TimePoint &dt,
                             const types::KeyValueMap &attributes)
        : Super(status::Error(
                    text,                    // text
                    status::Domain::DEVICE,  // domain
                    device,                  // origin
                    code,                    // code
                    id,                      // id
                    level,                   // level
                    dt,                      // timepoint
                    attributes),             // attributes
                std::runtime_error(text),
                TYPE_NAME_BASE(This))
    {
    }

    //==========================================================================
    // ServiceError

    ServiceError::ServiceError(const std::string &text,
                               const std::string &service,
                               Code code,
                               const std::string &id,
                               status::Level level,
                               const dt::TimePoint &dt,
                               const types::KeyValueMap &attributes)
        : Super(status::Error(
                    text,                     // text
                    status::Domain::SERVICE,  // domain
                    service,                  // origin
                    code,                     // code
                    id,                       // id
                    level,                    // level
                    dt,                       // timepoint
                    attributes),              // attributes
                std::runtime_error(text),
                TYPE_NAME_BASE(This))
    {
    }

    //==========================================================================
    // Error mapping methods

    status::Error::ptr map_to_error(const std::exception &e) noexcept
    {
        if (auto *ep = dynamic_cast<const status::Error *>(&e))
        {
            return std::make_shared<status::Error>(*ep);
        }
        else if (auto *ep = dynamic_cast<const fs::filesystem_error *>(&e))
        {
            return std::make_shared<FilesystemError>(*ep);
        }
        else if (auto *ep = dynamic_cast<const std::system_error *>(&e))
        {
            return std::make_shared<SystemError>(*ep);
        }
        else if (auto *ep = dynamic_cast<const std::invalid_argument *>(&e))
        {
            return std::make_shared<InvalidArgument>(ep->what());
        }
        else if (auto *ep = dynamic_cast<const std::logic_error *>(&e))
        {
            return std::make_shared<FailedPrecondition>(ep->what());
        }
        else if (auto *ep = dynamic_cast<const std::out_of_range *>(&e))
        {
            return std::make_shared<OutOfRange>(ep->what());
        }
        else if (auto *ep = dynamic_cast<const std::runtime_error *>(&e))
        {
            return std::make_shared<RuntimeError>(ep->what());
        }
        else
        {
            return std::make_shared<RuntimeError>(e.what());
        }
    }

    status::Error::ptr map_to_error(std::exception_ptr eptr) noexcept
    {
        if (eptr)
        {
            try
            {
                std::rethrow_exception(eptr);
            }
            catch (const std::exception &e)
            {
                return map_to_error(e);
            }
            catch (...)
            {
                return std::make_shared<status::Error>(
                    "Non-standard error",         // text
                    status::Domain::APPLICATION,  // domain
                    platform::path->exec_name(),  // origin
                    0,                            // code
                    "UNKNOWN",                    // symbol
                    status::Level::ERROR);       // level
            }
        }
        else
        {
            return {};
        }
    }
}  // namespace core::exception

namespace std
{
    /// Define output stream operator "<<" on std::exception and derivatives.
    std::ostream &operator<<(std::ostream &stream, const exception &e)
    {
        if (auto *ep = dynamic_cast<const core::status::Error *>(&e))
        {
            ep->to_stream(stream);
        }
        else
        {
            stream << e.what();
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, exception_ptr eptr)
    {
        if (eptr)
        {
            try
            {
                std::rethrow_exception(eptr);
            }
            catch (const core::status::Error &event)
            {
                event.to_stream(stream);
            }
            catch (const std::exception &e)
            {
                stream << e.what();
            }
            catch (...)
            {
                stream << "Uknonwn exception";
            }
        }
        return stream;
    }
}  // namespace std
