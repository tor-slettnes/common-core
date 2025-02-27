/// -*- c++ -*-
//==============================================================================
/// @file windows-logsink.c++
/// @brief OS native logger backend - WINDOWS variant
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "windows-logsink.h++"

// #using <system.dll>
// #using <mscorlib.dll>

namespace core::platform
{
    WindowsLogSinkProvider::WindowsLogSinkProvider(
        const std::string &application_id,
        const std::string &sink_id)
        : Super("WindowsLogSinkProvider", sink_id),
          application_id_(application_id)
    {
    }

    void WindowsLogSinkProvider::open()
    {
        Super::open();
        this->event_log = RegisterEventSource(NULL, this->application_id().c_str());
    }

    void WindowsLogSinkProvider::close()
    {
        DeregisterEventSource(this->event_log);
        Super::close();
    }

    bool WindowsLogSinkProvider::handle_message(const logging::Message::ptr &message)
    {
        if (auto *eventType = this->levelmap.get_ptr(message->level()))
        {
            const std::string &text = this->formatted(message);
            LPCSTR cstr = text.c_str();
            ReportEvent(this->event_log,  // hEventLog
                        *eventType,       // dwEventId
                        0,                // wCategory
                        0,                // dwEventID
                        nullptr,          // lpUserSid
                        1,                // wNumStrings
                        0,                // dwDataSize
                        &cstr,            // lpStrings
                        nullptr);         // lpRawData
            return true;
        }
        else
        {
            return false;
        }
    }

    std::string WindowsLogSinkProvider::application_id() const
    {
        return this->application_id_;
    }

    const types::ValueMap<status::Level, WORD> WindowsLogSinkProvider::levelmap = {
        {status::Level::INFO, EVENTLOG_INFORMATION_TYPE},
        {status::Level::NOTICE, EVENTLOG_INFORMATION_TYPE},
        {status::Level::WARNING, EVENTLOG_WARNING_TYPE},
        {status::Level::ERROR, EVENTLOG_ERROR_TYPE},
        {status::Level::CRITICAL, EVENTLOG_ERROR_TYPE},
        {status::Level::FATAL, EVENTLOG_ERROR_TYPE},
    };

}  // namespace core::platform
