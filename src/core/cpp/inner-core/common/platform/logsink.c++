/// -*- c++ -*-
//==============================================================================
/// @file logsink.c++
/// @brief OS native logger backend
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logsink.h++"

namespace core::platform
{
    LogSinkProvider::LogSinkProvider(const std::string &implementation,
                                     const std::string &sink_id)
        : LogSink(sink_id),
          Provider(implementation)
    {
    }

    void LogSinkProvider::load_settings(const types::KeyValueMap &settings)
    {
        Super::load_settings(settings);
        this->load_message_format(settings);
    }

    ProviderProxy<LogSinkProvider> logsink("logsink");
}  // namespace core::platform
