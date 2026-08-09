/// -*- c++ -*-
//==============================================================================
/// @file logsink.c++
/// @brief OS native logger backend
/// @author Tor Slettnes
//==============================================================================

#include "logsink.h++"

namespace cc::core::platform
{
    LogSinkProvider::LogSinkProvider(const std::string& implementation,
                                     const std::string& sink_id)
        : Super(sink_id),
          Provider(implementation)
    {
    }

    ProviderProxy<LogSinkProvider> logsink("logsink");
}  // namespace cc::core::platform
