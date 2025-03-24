// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-timezone.c++
/// @brief SysConfig gRPC implementation - time zone settings
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-grpc-timezone.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace sysconfig::grpc
{
    TimeZoneProvider::TimeZoneProvider(const std::shared_ptr<Client> &client)
        : Super(TYPE_NAME_FULL(This)),
          client(client)
    {
    }

    void TimeZoneProvider::initialize()
    {
        Super::initialize();
        this->client->add_handler(
            ::cc::sysconfig::Signal::kTzConfig,
            [&](const ::cc::sysconfig::Signal &signal) {
                sysconfig::signal_tzconfig.emit(
                    protobuf::decoded<TimeZoneCanonicalName>(signal.tz_config()));
            });

        this->client->add_handler(
            ::cc::sysconfig::Signal::kTzInfo,
            [&](const ::cc::sysconfig::Signal &signal) {
                sysconfig::signal_tzinfo.emit(
                    protobuf::decoded<TimeZoneInfo>(signal.tz_info()));
            });
    }

    TimeZoneAreas TimeZoneProvider::list_timezone_areas() const
    {
        ::cc::sysconfig::TimeZoneAreas response = this->client->call_check(
            &Client::Stub::list_timezone_areas);

        return {
            response.areas().begin(),
            response.areas().end(),
        };
    }

    TimeZoneCountries TimeZoneProvider::list_timezone_countries(
        const TimeZoneArea &area)
    {
        ::cc::sysconfig::TimeZoneCountries response = this->client->call_check(
            &Client::Stub::list_timezone_countries,
            protobuf::encoded<::cc::sysconfig::TimeZoneArea>(area));

        return protobuf::decode_to_vector<TimeZoneCountry>(response.countries());
    }

    TimeZoneRegions TimeZoneProvider::list_timezone_regions(
        const TimeZoneLocationFilter &filter)
    {
        ::cc::sysconfig::TimeZoneRegions response = this->client->call_check(
            &Client::Stub::list_timezone_regions,
            protobuf::encoded<::cc::sysconfig::TimeZoneLocationFilter>(filter));

        return {
            response.regions().begin(),
            response.regions().end(),
        };
    }

    TimeZoneCanonicalSpecs TimeZoneProvider::list_timezone_specs(
        const TimeZoneLocationFilter &filter) const
    {
        ::cc::sysconfig::TimeZoneCanonicalSpecs response =
            this->client->call_check(
                &Client::Stub::list_timezone_specs,
                protobuf::encoded<::cc::sysconfig::TimeZoneLocationFilter>(filter));

        return protobuf::decode_to_vector<TimeZoneCanonicalSpec>(response.specs());
    }

    TimeZoneCanonicalSpec TimeZoneProvider::get_timezone_spec(
        const TimeZoneCanonicalName &zone) const
    {
        return protobuf::decoded<TimeZoneCanonicalSpec>(
            this->client->call_check(
                &Client::Stub::get_timezone_spec,
                protobuf::encoded<::cc::sysconfig::TimeZoneCanonicalName>(zone)));
    }

    TimeZoneInfo TimeZoneProvider::get_timezone_info(
        const TimeZoneCanonicalName &canonical_zone,
        const core::dt::TimePoint &timepoint) const
    {
        return protobuf::decoded<TimeZoneInfo>(
            this->client->call_check(
                &Client::Stub::get_timezone_info,
                protobuf::encoded<::cc::sysconfig::TimeZoneInfoRequest>(
                    canonical_zone,
                    timepoint)));
    }

    TimeZoneInfo TimeZoneProvider::set_timezone(
        const TimeZoneCanonicalName &zone)
    {
        ::cc::sysconfig::TimeZoneConfig request;
        request.set_canonical_zone(zone);

        return protobuf::decoded<TimeZoneInfo>(
            this->client->call_check(&Client::Stub::set_timezone, request));
    }

    TimeZoneInfo TimeZoneProvider::set_timezone(
        const TimeZoneLocation &location)
    {
        ::cc::sysconfig::TimeZoneConfig request;
        protobuf::encode(location, request.mutable_location());

        return protobuf::decoded<TimeZoneInfo>(
            this->client->call_check(&Client::Stub::set_timezone, request));
    }

}  // namespace sysconfig::grpc
