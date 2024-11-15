/// -*- c++ -*-
//==============================================================================
/// @file logsink.h++
/// @brief OS native logger backend - Abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "logging/sinks/factory.h++"
#include "logging/sinks/logsink.h++"
#include "logging/sinks/messageformatter.h++"

#include <string>

namespace core::platform
{
    //--------------------------------------------------------------------------
    // \class LogSinkProvider

    class LogSinkProvider : public logging::LogSink,
                            public logging::MessageFormatter,
                            public Provider
    {
        using Super = logging::LogSink;

    protected:
        /// @param[in] implementation
        ///     Implementation name, e.g. final class name
        /// @param[in] sink_id
        ///     Sink identifier
        LogSinkProvider(const std::string &implementation,
                        const std::string &sink_id = "syslog");

    protected:
        void load_settings(const types::KeyValueMap &settings) override;
        bool is_applicable(const types::Loggable &item) const override;
    };

    //--------------------------------------------------------------------------
    // Global provider instance

    extern ProviderProxy<LogSinkProvider> logsink;

    //--------------------------------------------------------------------------
    // Add sink to factory

    inline static logging::SinkFactory syslog_factory(
        "syslog",
        "Log via UNIX syslog.",
        [](const logging::SinkID &sink_id) -> logging::Sink::ptr {
            return logsink.get_shared();
        },
        logging::DefaultOption::UNLESS_INTERACTIVE);

}  // namespace core::platform
