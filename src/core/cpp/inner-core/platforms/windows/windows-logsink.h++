/// -*- c++ -*-
//==============================================================================
/// @file windows-logsink.h++
/// @brief OS native logger backend - WINDOWS variant
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "platform/logsink.h++"
#include "types/valuemap.h++"
#include "status/level.h++"

#include <windows.h>

namespace core::platform
{
    class WindowsLogSinkProvider : public LogSinkProvider
    {
        using This = WindowsLogSinkProvider;
        using Super = LogSinkProvider;

    protected:
        WindowsLogSinkProvider(const std::string &application_id,
                               const std::string &sink_id = "eventlog");

    public:
        void open() override;
        void close() override;
        bool handle_message(const logging::Message::ptr &message) override;
        std::string application_id() const;

    private:
        static const types::ValueMap<status::Level, WORD> levelmap;
        std::string application_id_;
        HANDLE event_log;
    };

}  // namespace core::platform
