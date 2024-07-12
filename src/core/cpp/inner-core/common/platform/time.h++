/// -*- c++ -*-
//==============================================================================
/// @file time.h++
/// @brief Date/Time functions - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "chrono/date-time.h++"

namespace core::platform
{
    class TimeProvider : public Provider
    {
        using This = TimeProvider;
        using Super = Provider;

    protected:
        using Super::Super;

    public:
        virtual void set_time(const dt::TimePoint &tp) = 0;

        // virtual void set_time_config(bool ntp,
        //                              const std::vector<std::string> &servers) = 0;

        // virtual void set_time_config(const dt::TimePoint &tp) = 0;
    };

    extern ProviderProxy<TimeProvider> time;
}  // namespace core::platform
