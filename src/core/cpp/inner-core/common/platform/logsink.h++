/// -*- c++ -*-
//==============================================================================
/// @file logsink.h++
/// @brief OS native logger backend - Abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "logging/sinks/messagesink.h++"

#include <string>

namespace cc::platform
{
    class LogSinkProvider : public logging::MessageSink,
                            public Provider
    {
        using Super = logging::MessageSink;

    protected:
        /// @param[in] implementation
        ///     Implementation name, e.g. final class name
        /// @param[in] identity
        ///     Logging entity name, e.g. application name
        LogSinkProvider(const std::string &implementation,
                        const std::string &identity);

        const std::string &identity() const;

    protected:
        std::string identity_;
    };

    extern ProviderProxy<LogSinkProvider> logsink;
}  // namespace cc::platform
