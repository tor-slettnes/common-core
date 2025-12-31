/// -*- c++ -*-
//==============================================================================
/// @file logsink.h++
/// @brief OS native logger backend - Abstract interface
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "provider.h++"
#include "logging/sinks/factory.h++"
#include "logging/sinks/messagesink.h++"

#include <string>

namespace core::platform
{
    //--------------------------------------------------------------------------
    // @class LogSinkProvider

    class LogSinkProvider : public logging::MessageSink,
                            public Provider
    {
        using Super = logging::MessageSink;

    protected:
        /// @param[in] implementation
        ///     Implementation name, e.g. final class name
        /// @param[in] sink_id
        ///     Sink identifier
        LogSinkProvider(const std::string &implementation,
                        const std::string &sink_id = "syslog");
    };

    //--------------------------------------------------------------------------
    // Global provider instance

    extern ProviderProxy<LogSinkProvider> logsink;

    //--------------------------------------------------------------------------
    // Add sink to factory

    inline static logging::SinkFactory system_logger_factory(
        "syslog",
        "Log via UNIX syslog.",
        [](const logging::SinkID &sink_id) -> logging::Sink::ptr {
            return logsink.get_shared();
        },
        logging::DefaultOption::UNLESS_INTERACTIVE);

}  // namespace core::platform
