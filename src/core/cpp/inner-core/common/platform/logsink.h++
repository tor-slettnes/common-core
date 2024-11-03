/// -*- c++ -*-
//==============================================================================
/// @file logsink.h++
/// @brief OS native logger backend - Abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "logging/sinks/logsink.h++"
#include "logging/sinks/messageformatter.h++"

#include <string>

namespace core::platform
{
    class LogSinkProvider : public logging::LogSink,
                            public logging::MessageFormatter,
                            public Provider
    {
        using Super = logging::LogSink;

    protected:
        /// @param[in] implementation
        ///     Implementation name, e.g. final class name
        /// @param[in] identity
        ///     Logging entity name, e.g. application name
        LogSinkProvider(const std::string &implementation,
                        const std::string &sink_id = "syslog",
                        status::Level threshold = status::Level::NONE);
    };

    extern ProviderProxy<LogSinkProvider> logsink;
}  // namespace core::platform
