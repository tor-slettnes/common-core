/// -*- c++ -*-
//==============================================================================
/// @file posix-logsink.h++
/// @brief OS native logger backend - POSIX variant
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "platform/logsink.h++"
#include "types/valuemap.h++"
#include "status/level.h++"

namespace cc::core::platform
{
    //--------------------------------------------------------------------------
    // PosixLogSinkProvider

    class PosixLogSinkProvider : public LogSinkProvider
    {
        using This = PosixLogSinkProvider;
        using Super = LogSinkProvider;

    protected:
        PosixLogSinkProvider(
            const fs::path &exec_path,
            const std::string &sink_id = "syslog");

    public:
        void open() override;
        void close() override;
        bool handle_message(const logging::Message::ptr &message) override;
        const std::string &application_id() const;

    private:
        static const core::types::ValueMap<status::Level, int> levelmap;
        std::string application_id_;
    };
}  // namespace cc::core::platform
