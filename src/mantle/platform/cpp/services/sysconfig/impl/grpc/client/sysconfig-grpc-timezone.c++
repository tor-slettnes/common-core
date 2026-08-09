// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-timezone.c++
/// @brief SysConfig gRPC implementation - time zone settings
/// @author Tor Slettnes
//==============================================================================

#include "sysconfig-grpc-timezone.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace cc::platform::sysconfig::grpc
{
    TimeZoneProvider::TimeZoneProvider(const std::shared_ptr<Client>& client)
        : Super(TYPE_NAME_FULL(This)),
          client(client)
    {
    }

    void TimeZoneProvider::initialize()
    {
        Super::initialize();
        this->client->add_handler(
            platform::sysconfig::protobuf::Signal::kTzSpec,
            [&](const platform::sysconfig::protobuf::Signal& signal) {
                sysconfig::signal_tzspec.emit(
                    cc::protobuf::decoded<TimeZoneCanonicalSpec>(signal.tz_spec()));
            });

        this->client->add_handler(
            platform::sysconfig::protobuf::Signal::kTzInfo,
            [&](const platform::sysconfig::protobuf::Signal& signal) {
                sysconfig::signal_tzinfo.emit(
                    cc::protobuf::decoded<TimeZoneInfo>(signal.tz_info()));
            });
    }

    TimeZoneAreas TimeZoneProvider::list_timezone_areas() const
    {
        platform::sysconfig::protobuf::TimeZoneAreas response = this->client->call_check(
            &Client::Stub::ListTimezoneAreas);

        return {
            response.areas().begin(),
            response.areas().end(),
        };
    }

    TimeZoneCountries TimeZoneProvider::list_timezone_countries(
        const TimeZoneArea& area)
    {
        platform::sysconfig::protobuf::TimeZoneCountries response = this->client->call_check(
            &Client::Stub::ListTimezoneCountries,
            cc::protobuf::encoded<platform::sysconfig::protobuf::TimeZoneArea>(area));

        return cc::protobuf::decode_to_vector<TimeZoneCountry>(response.countries());
    }

    TimeZoneRegions TimeZoneProvider::list_timezone_regions(
        const TimeZoneLocationFilter& filter)
    {
        platform::sysconfig::protobuf::TimeZoneRegions response = this->client->call_check(
            &Client::Stub::ListTimezoneRegions,
            cc::protobuf::encoded<platform::sysconfig::protobuf::TimeZoneLocationFilter>(filter));

        return {
            response.regions().begin(),
            response.regions().end(),
        };
    }

    TimeZoneCanonicalSpecs TimeZoneProvider::list_timezone_specs(
        const TimeZoneLocationFilter& filter) const
    {
        platform::sysconfig::protobuf::TimeZoneCanonicalSpecs response =
            this->client->call_check(
                &Client::Stub::ListTimezoneSpecs,
                cc::protobuf::encoded<platform::sysconfig::protobuf::TimeZoneLocationFilter>(filter));

        return cc::protobuf::decode_to_vector<TimeZoneCanonicalSpec>(response.specs());
    }

    TimeZoneCanonicalSpec TimeZoneProvider::get_timezone_spec(
        const TimeZoneCanonicalName& zone) const
    {
        return cc::protobuf::decoded<TimeZoneCanonicalSpec>(
            this->client->call_check(
                &Client::Stub::GetTimezoneSpec,
                cc::protobuf::encoded<platform::sysconfig::protobuf::TimeZoneCanonicalName>(zone)));
    }

    TimeZoneInfo TimeZoneProvider::get_timezone_info(
        const TimeZoneCanonicalName& canonical_zone,
        const core::dt::TimePoint& timepoint) const
    {
        return cc::protobuf::decoded<TimeZoneInfo>(
            this->client->call_check(
                &Client::Stub::GetTimezoneInfo,
                cc::protobuf::encoded<platform::sysconfig::protobuf::TimeZoneInfoRequest>(
                    canonical_zone,
                    timepoint)));
    }

    TimeZoneInfo TimeZoneProvider::set_timezone(
        const TimeZoneCanonicalName& zone)
    {
        platform::sysconfig::protobuf::TimeZoneConfig request;
        request.set_canonical_zone(zone);

        return cc::protobuf::decoded<TimeZoneInfo>(
            this->client->call_check(&Client::Stub::SetTimezone, request));
    }

    TimeZoneInfo TimeZoneProvider::set_timezone(
        const TimeZoneLocation& location)
    {
        platform::sysconfig::protobuf::TimeZoneConfig request;
        cc::protobuf::encode(location, request.mutable_location());

        return cc::protobuf::decoded<TimeZoneInfo>(
            this->client->call_check(&Client::Stub::SetTimezone, request));
    }

}  // namespace cc::platform::sysconfig::grpc
