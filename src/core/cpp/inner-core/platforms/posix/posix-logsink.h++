/// -*- c++ -*-
//==============================================================================
/// @file posix-logsink.h++
/// @brief OS native logger backend - POSIX variant
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/logsink.h++"
#include "types/valuemap.h++"
#include "status/level.h++"

namespace core::platform
{
    class PosixLogSinkProvider : public LogSinkProvider
    {
        using This = PosixLogSinkProvider;
        using Super = LogSinkProvider;

    protected:
        PosixLogSinkProvider(const std::string &application_id,
                             const std::string &sink_id = "syslog");

    public:
        void open() override;
        void close() override;
        void capture_event(const status::Event::ptr &event) override;
        std::string application_id() const;

    private:
        static const core::types::ValueMap<status::Level, int> levelmap;
        std::string application_id_;
    };

}  // namespace core::platform
