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

namespace cc::platform
{
    class PosixLogSinkProvider : public LogSinkProvider
    {
        using This = PosixLogSinkProvider;
        using Super = LogSinkProvider;

    protected:
        PosixLogSinkProvider(const std::string &identity);

    public:
        void open() override;
        void close() override;
        void capture_message(const logging::Message::Ref &msg) override;

    private:
        static const types::ValueMap<status::Level, int> levelmap;
    };

}  // namespace cc::platform
