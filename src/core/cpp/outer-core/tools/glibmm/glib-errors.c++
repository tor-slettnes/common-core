/// -*- c++ -*-
//==============================================================================
/// @file glib-errors.c++
/// @brief GLib Error wrappers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "glib-errors.h++"
#include "status/exceptions.h++"

#include <system_error>

namespace core::glib
{
    //==========================================================================
    // Error_Category

    Error_Category::Error_Category(const std::string &classname)
        : classname_(classname)
    {
    }

    const char *Error_Category::name() const noexcept
    {
        return this->classname_.c_str();
    }

    std::string Error_Category::message(int condition) const
    {
        return str::format("%s::%d", this->classname_, condition);
    }

    const Error_Category glib_error_category("Glib::Error");
    const Error_Category gio_dbus_error_category("Gio::DBus::Error");

    //==========================================================================
    // Error

    Error::Error(const Glib::Error &e,
                 const std::optional<std::string> &preamble)
        : std::system_error(
              e.code(),
              glib_error_category,
              (preamble ? (*preamble + ": ") : ""s) + e.what())
    {
    }

    Error::Error(const Gio::DBus::Error &e,
                 const std::optional<std::string> &preamble)
        : std::system_error(
              e.code(),
              gio_dbus_error_category,
              (preamble ? (*preamble + ": ") : ""s) + e.what())
    {
    }

    void log_exception(std::exception_ptr eptr,
                       const std::string &preamble,
                       core::status::Level level,
                       core::logging::Scope::Ref scope,
                       core::status::Flow flow,
                       const dt::TimePoint &tp,
                       const std::filesystem::path &path,
                       const int &lineno,
                       const std::string &function)
    {
        if (eptr)
        {
            core::logging::MessageBuilder::Ref msg = custom_log_msg(
                level, scope, flow, tp, path, lineno, function);

            if (preamble.size())
            {
                msg->add(preamble, ": ");
            }

            try
            {
                std::rethrow_exception(eptr);
            }
            catch (const std::exception &e)
            {
                msg->add(e);
            }
            catch (const Gio::DBus::Error &e)
            {
                msg->format("[Gio::Error::%s]: %s", e.code(), e.what());
            }
            catch (const Glib::Error &e)
            {
                msg->format("[Glib::Error::%s]: %s", e.code(), e.what());
            }
            catch (...)
            {
                msg->add("Unknown exception");
            }

            msg->dispatch();
        }
    }
}  // namespace gwrap


namespace Glib
{
    std::ostream &operator<<(std::ostream &stream, const Error &e)
    {
        stream << e.what();
        return stream;
    }
}
