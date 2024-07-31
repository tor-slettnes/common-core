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

        virtual void set_ntp(bool ntp);
        virtual bool get_ntp() const;

        virtual void set_ntp_servers(const std::vector<std::string> &servers);
        virtual std::vector<std::string> get_ntp_servers() const;

    };

    extern ProviderProxy<TimeProvider> time;
}  // namespace core::platform
