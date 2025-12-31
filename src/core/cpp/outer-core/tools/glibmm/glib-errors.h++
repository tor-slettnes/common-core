/// -*- c++ -*-
//==============================================================================
/// @file glib-errors.h++
/// @brief GLib Error wrappers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "logging/logging.h++"

#include <giomm/dbuserror.h>
#include <system_error>
#include <optional>

namespace core::glib
{
    define_log_scope("glib");

    //==========================================================================
    // Error_Category

    class Error_Category : public std::error_category
    {
    public:
        Error_Category(const std::string& classname);

        const char* name() const noexcept override;

        std::string message(int condition = 0) const override;

    private:
        const std::string classname_;
    };

    extern const Error_Category glib_error_category;
    extern const Error_Category gio_dbus_error_category;

    //==========================================================================
    // Error

    class Error : public std::system_error
    {
    public:
        Error(const Glib::Error& e,
              const std::optional<std::string>& preamble = {});

        Error(const Gio::DBus::Error& e,
              const std::optional<std::string>& preamble = {});
    };

    //==========================================================================
    // @brief Log information from an std::exception_ptr capture

    void log_exception(std::exception_ptr eptr,
                       const std::string& preamble = {},
                       core::status::Level level = core::status::Level::NOTICE,
                       core::logging::Scope::ptr scope = log_scope,
                       const dt::TimePoint& tp = dt::Clock::now(),
                       const std::filesystem::path& path = __builtin_FILE(),
                       const int& lineno = __builtin_LINE(),
                       const std::string& function = __builtin_FUNCTION());

}  // namespace core::glib

/// Output stream support for Glib VariantBase, for use in log output.
namespace Glib
{
    std::ostream& operator<<(std::ostream& stream, const Error& e);
}  // namespace Glib
