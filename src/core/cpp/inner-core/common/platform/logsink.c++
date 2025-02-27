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
        : Super(sink_id, false),
          Provider(implementation)
    {
    }

    ProviderProxy<LogSinkProvider> logsink("logsink");
}  // namespace core::platform
