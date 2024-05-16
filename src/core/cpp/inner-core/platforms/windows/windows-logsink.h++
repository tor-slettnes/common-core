/// -*- c++ -*-
//==============================================================================
/// @file windows-logsink.h++
/// @brief OS native logger backend - WINDOWS variant
/// @author Tor Slettnes <tor@slett.net>
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
        WindowsLogSinkProvider(const std::string &identity);

    public:
        void open() override;
        void close() override;
        void capture_message(const logging::Message::Ref &msg) override;

    private:
        static const types::ValueMap<status::Level, WORD> levelmap;
        HANDLE event_log;
    };

}  // namespace core::platform
