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
    WindowsLogSinkProvider::WindowsLogSinkProvider(const std::string &identity)
        : Super("WindowsLogSinkProvider", identity)
    {
    }

    void WindowsLogSinkProvider::open()
    {
        Super::open();
        this->event_log = RegisterEventSource(NULL, this->identity().c_str());
    }

    void WindowsLogSinkProvider::close()
    {
        DeregisterEventSource(this->event_log);
        Super::close();
    }

    void WindowsLogSinkProvider::capture_event(const status::Event::ptr &event)
    {
        if (auto *eventType = this->levelmap.get_ptr(event->level()))
        {
            const std::string &text = this->formatted(event);
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
        }
    }

    const types::ValueMap<status::Level, WORD> WindowsLogSinkProvider::levelmap = {
        {status::Level::INFO, EVENTLOG_INFORMATION_TYPE},
        {status::Level::NOTICE, EVENTLOG_INFORMATION_TYPE},
        {status::Level::WARNING, EVENTLOG_WARNING_TYPE},
        {status::Level::FAILED, EVENTLOG_ERROR_TYPE},
        {status::Level::CRITICAL, EVENTLOG_ERROR_TYPE},
        {status::Level::FATAL, EVENTLOG_ERROR_TYPE},
    };

}  // namespace core::platform
