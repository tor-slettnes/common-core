/// -*- c++ -*-
//==============================================================================
/// @file logsink.c++
/// @brief OS native logger backend
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logsink.h++"

namespace shared::platform
{
    LogSinkProvider::LogSinkProvider(const std::string &implementation,
                                     const std::string &identity)
        : logging::MessageSink(),
          Provider(implementation),
          identity_(identity)
    {
    }

    const std::string &LogSinkProvider::identity() const
    {
        return this->identity_;
    }

    ProviderProxy<LogSinkProvider> logsink("logsink");
}  // namespace shared::platform
