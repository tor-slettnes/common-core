// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-timezone.c++
/// @brief SysConfig service - Time Zone Configuration - gRPC Implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-grpc-timezone.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace platform::sysconfig::grpc
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
            cc::platform::sysconfig::Signal::kTzConfig,
            [&](const cc::platform::sysconfig::Signal &signal) {
                platform::sysconfig::signal_tzconfig.emit(
                    protobuf::decoded<TimeZoneConfig>(signal.tz_config()));
            });

        this->client->add_handler(
            cc::platform::sysconfig::Signal::kTzInfo,
            [&](const cc::platform::sysconfig::Signal &signal) {
                platform::sysconfig::signal_tzinfo.emit(
                    protobuf::decoded<TimeZoneInfo>(signal.tz_info()));
            });
    }

    TimeZoneAreas TimeZoneProvider::list_timezone_areas() const
    {
        return protobuf::assign_to_vector<std::string>(
            this->client->call_check(&Client::Stub::list_timezone_areas).areas());
    }

    TimeZoneCountries TimeZoneProvider::list_timezone_countries(
        const TimeZoneArea &area)
    {
        return protobuf::decode_to_vector<TimeZoneCountry>(
            this->client->call_check(
                            &Client::Stub::list_timezone_countries,
                            protobuf::encoded<cc::platform::sysconfig::TimeZoneArea>(area))
                .countries());
    }

    TimeZoneCanonicalSpecs TimeZoneProvider::list_timezone_specs(
        const TimeZoneLocationFilter &filter) const
    {
        cc::platform::sysconfig::TimeZoneCanonicalSpecs response =
            this->client->call_check(
                &Client::Stub::list_timezone_specs,
                protobuf::encoded<cc::platform::sysconfig::TimeZoneLocationFilter>(filter));

        return protobuf::decode_to_vector<TimeZoneCanonicalSpec>(response.specs());
    }

    TimeZoneCanonicalSpec TimeZoneProvider::get_timezone_spec(
        const TimeZoneCanonicalName &zone) const
    {
        cc::platform::sysconfig::TimeZoneCanonicalName request;
        request.set_name(zone);

        return protobuf::decoded<TimeZoneCanonicalSpec>(
            this->client->call_check(&Client::Stub::get_timezone_spec, request));
    }

    TimeZoneInfo TimeZoneProvider::set_timezone(
        const TimeZoneConfig &config)
    {
        return protobuf::decoded<TimeZoneInfo>(
            this->client->call_check(
                &Client::Stub::set_timezone,
                protobuf::encoded<cc::platform::sysconfig::TimeZoneConfig>(config)));
    }

    TimeZoneConfig TimeZoneProvider::get_timezone_config() const
    {
        return protobuf::decoded<TimeZoneConfig>(
            this->client->call_check(
                &Client::Stub::get_timezone_config));
    }

    TimeZoneInfo TimeZoneProvider::get_timezone_info(
        const TimeZoneCanonicalName &canonical_zone,
        const core::dt::TimePoint &timepoint) const
    {
        return protobuf::decoded<TimeZoneInfo>(
            this->client->call_check(
                &Client::Stub::get_timezone_info,
                protobuf::encoded<cc::platform::sysconfig::TimeZoneInfoRequest>(
                    canonical_zone,
                    timepoint)));
    }

}  // namespace platform::sysconfig::grpc
