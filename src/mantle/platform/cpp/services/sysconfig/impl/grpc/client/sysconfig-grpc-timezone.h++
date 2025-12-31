// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-timezone.h++
/// @brief SysConfig gRPC implementation - time zone settings
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "sysconfig-timezone.h++"
#include "sysconfig-grpc-client.h++"

namespace sysconfig::grpc
{
    class TimeZoneProvider : public TimeZoneInterface
    {
        using This = TimeZoneProvider;
        using Super = TimeZoneInterface;

    public:
        TimeZoneProvider(const std::shared_ptr<Client> &client);

        void initialize() override;

    protected:
        //==========================================================================
        // Time zone configuration

        TimeZoneAreas list_timezone_areas() const override;

        TimeZoneCountries list_timezone_countries(
            const TimeZoneArea &area) override;

        TimeZoneRegions list_timezone_regions(
            const TimeZoneLocationFilter &filter) override;

        TimeZoneCanonicalSpecs list_timezone_specs(
            const TimeZoneLocationFilter &filter) const override;

        TimeZoneCanonicalSpec get_timezone_spec(
            const TimeZoneCanonicalName &zone) const override;

        TimeZoneInfo get_timezone_info(
            const TimeZoneCanonicalName &canonical_zone = {},
            const core::dt::TimePoint &timepoint = {}) const override;

        TimeZoneInfo set_timezone(
            const TimeZoneCanonicalName &zone) override;

        TimeZoneInfo set_timezone(
            const TimeZoneLocation &location) override;

    private:
        std::shared_ptr<Client> client;
    };
}  // namespace sysconfig::grpc
