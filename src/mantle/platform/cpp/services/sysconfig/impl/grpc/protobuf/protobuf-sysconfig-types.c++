/// -*- c++ -*-
//==============================================================================
/// @file protobuf-sysconfig-types.c++
/// @brief Encode/decode routines for types in `sysconfig.proto`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-sysconfig-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace protobuf
{

    //==========================================================================
    // HostInfo

    void encode(const platform::sysconfig::HostInfo &native,
                cc::platform::sysconfig::HostInfo *proto) noexcept
    {
        proto->set_hostname(native.hostname);
        proto->set_os_name(native.os_name);
        proto->set_os_release_flavor_short(native.os_release_flavor_short);
        proto->set_os_release_flavor_long(native.os_release_flavor_long);
        proto->set_os_release_version_id(native.os_release_version_id);
        proto->set_os_release_version_name(native.os_release_version_name);
        proto->set_machine_arch(native.machine_arch);
        proto->set_machine_cpu(native.machine_cpu);
        proto->set_machine_name(native.machine_name);
        proto->set_machine_description(native.machine_description);
        proto->set_machine_serial(native.machine_serial);
        proto->set_machine_uid(native.machine_uid);
    }

    void decode(const cc::platform::sysconfig::HostInfo &proto,
                platform::sysconfig::HostInfo *native) noexcept
    {
        native->hostname = proto.hostname();
        native->os_name = proto.os_name();
        native->os_release_flavor_short = proto.os_release_flavor_short();
        native->os_release_flavor_long = proto.os_release_flavor_long();
        native->os_release_version_id = proto.os_release_version_id();
        native->os_release_version_name = proto.os_release_version_name();

        native->machine_arch = proto.machine_arch();
        native->machine_cpu = proto.machine_cpu();
        native->machine_name = proto.machine_name();
        native->machine_description = proto.machine_description();
        native->machine_serial = proto.machine_serial();
        native->machine_uid = proto.machine_uid();
    }

    //==========================================================================
    // ProductInfo

    void encode(const platform::sysconfig::ProductInfo &native,
                cc::platform::sysconfig::ProductInfo *proto) noexcept
    {
        proto->set_product_model(native.product_model);
        proto->set_product_serial(native.product_serial);
        proto->set_hardware_model(native.hardware_model);
        encode(native.release_version, proto->mutable_release_version());
        encode(native.component_versions, proto->mutable_component_versions());
        for (const platform::sysconfig::ProductInfo &si : native.subsystem_info)
        {
            encode(si, proto->add_subsystem_info());
        }
        // protobuf::encode_list<cc::platform::sysconfig::ProductInfo>(
        //     native.subsystem_info.cbegin(),
        //     native.subsystem_info.cend(),
        //     proto->mutable_subsystem_info());
    }

    void decode(const cc::platform::sysconfig::ProductInfo &proto,
                platform::sysconfig::ProductInfo *native) noexcept
    {
        native->product_model = proto.product_model();
        native->product_serial = proto.product_serial();
        native->hardware_model = proto.hardware_model();
        decode(proto.release_version(), &native->release_version);
        decode(proto.component_versions(), &native->component_versions);
        native->subsystem_info.reserve(proto.subsystem_info_size());

        for (const cc::platform::sysconfig::ProductInfo &si : proto.subsystem_info())
        {
            decode(si, &native->subsystem_info.emplace_back());
        }
    }

    //==========================================================================
    // Version

    void encode(const platform::sysconfig::Version &native,
                cc::version::Version *proto) noexcept
    {
        proto->set_major(native.major);
        proto->set_minor(native.minor);
        proto->set_tweak(native.tweak);
        proto->set_printable_version(native.printable_version);
    }

    void decode(const cc::version::Version &proto,
                platform::sysconfig::Version *native) noexcept
    {
        native->major = proto.major();
        native->minor = proto.minor();
        native->tweak = proto.tweak();
        native->printable_version = proto.printable_version();
    }

    //==========================================================================
    // ComponentVersions

    void encode(const platform::sysconfig::ComponentVersions &native,
                cc::version::ComponentVersions *proto) noexcept
    {
        google::protobuf::Map<std::string, cc::version::Version> *cv = proto->mutable_components();

        for (const auto &[component, version] : native)
        {
            encode(version, &(*cv)[component]);
        }
    }

    void decode(const cc::version::ComponentVersions &proto,
                platform::sysconfig::ComponentVersions *native) noexcept
    {
        for (const auto &[component, version] : proto.components())
        {
            decode(version, &(*native)[component]);
        }
    }

    //==========================================================================
    // TimeZoneCanonicalSpec

    void encode(const platform::sysconfig::TimeZoneCanonicalSpec &native,
                cc::platform::sysconfig::TimeZoneCanonicalSpec *proto) noexcept
    {
        proto->set_name(native.name);
        proto->set_area(native.area);

        encode_vector<cc::platform::sysconfig::TimeZoneCountryRegion>(
            native.countries,
            proto->mutable_countries());

        proto->set_latitude(native.latitude);
        proto->set_longitude(native.longitude);
    }

    void decode(const cc::platform::sysconfig::TimeZoneCanonicalSpec &proto,
                platform::sysconfig::TimeZoneCanonicalSpec *native) noexcept
    {
        native->name = proto.name();
        native->area = proto.area();
        decode_to_vector<platform::sysconfig::TimeZoneCountryRegion>(
            proto.countries(),
            &native->countries);
        native->latitude = proto.latitude();
        native->longitude = proto.longitude();
    }

    //==========================================================================
    // TimeLocationFilter

    void encode(const platform::sysconfig::TimeZoneLocationFilter &native,
                cc::platform::sysconfig::TimeZoneLocationFilter *proto) noexcept
    {
        proto->set_area(native.area);
        if (!native.country.code.empty())
        {
            proto->set_country_code(native.country.code);
        }
        else if (!native.country.name.empty())
        {
            proto->set_country_name(native.country.name);
        }
    }

    void decode(const cc::platform::sysconfig::TimeZoneLocationFilter &proto,
                platform::sysconfig::TimeZoneLocationFilter *native) noexcept
    {
        native->area = proto.area();
        if (proto.has_country_code())
        {
            native->country.code = proto.country_code();
        }
        else if (proto.has_country_name())
        {
            native->country.name = proto.country_name();
        }
    }

    //==========================================================================
    // TimeZoneArea

    void encode(const platform::sysconfig::TimeZoneArea &native,
                cc::platform::sysconfig::TimeZoneArea *proto) noexcept
    {
        proto->set_name(native);
    }

    void decode(const cc::platform::sysconfig::TimeZoneArea &proto,
                platform::sysconfig::TimeZoneArea *native) noexcept
    {
        *native = proto.name();
    }

    //==========================================================================
    // TimeZoneCountry

    void encode(const platform::sysconfig::TimeZoneCountry &native,
                cc::platform::sysconfig::TimeZoneCountry *proto) noexcept
    {
        proto->set_code(native.code);
        proto->set_name(native.name);
    }

    void decode(const cc::platform::sysconfig::TimeZoneCountry &proto,
                platform::sysconfig::TimeZoneCountry *native) noexcept
    {
        native->code = proto.code();
        native->name = proto.name();
    }

    //==========================================================================
    // TimeZoneCountryRegion

    void encode(const platform::sysconfig::TimeZoneCountryRegion &native,
                cc::platform::sysconfig::TimeZoneCountryRegion *proto) noexcept
    {
        encode(native.country, proto->mutable_country());
        proto->set_region(native.region);
    }

    void decode(const cc::platform::sysconfig::TimeZoneCountryRegion &proto,
                platform::sysconfig::TimeZoneCountryRegion *native) noexcept
    {
        decode(proto.country(), &native->country);
        native->region = proto.region();
    }

    //==========================================================================
    // TimeZoneConfig

    void encode(const platform::sysconfig::TimeZoneConfig &native,
                cc::platform::sysconfig::TimeZoneConfig *proto) noexcept
    {
        proto->set_zonename(native.zonename);
        proto->set_automatic(native.automatic);
        proto->set_provider(native.provider);
    }

    void decode(const cc::platform::sysconfig::TimeZoneConfig &proto,
                platform::sysconfig::TimeZoneConfig *native) noexcept
    {
        native->zonename = proto.zonename();
        native->automatic = proto.automatic();
        native->provider = proto.provider();
    }

    //==========================================================================
    // TimeZoneInfo

    void encode(const core::dt::TimeZoneInfo &native,
                cc::platform::sysconfig::TimeZoneInfo *proto) noexcept
    {
        proto->set_shortname(native.shortname);
        encode(native.offset, proto->mutable_offset());
        encode(native.stdoffset, proto->mutable_stdoffset());
        proto->set_dst(native.dst);
    }


    void decode(const cc::platform::sysconfig::TimeZoneInfo &proto,
                core::dt::TimeZoneInfo *native) noexcept
    {
        native->shortname = proto.shortname();
        decode(proto.offset(), &native->offset);
        decode(proto.stdoffset(), &native->stdoffset);
        native->dst = proto.dst();
    }

    //==========================================================================
    // TimeZoneInfoRequest

    void encode(const platform::sysconfig::TimeZoneCanonicalName &canonical_zone,
                const core::dt::TimePoint &timepoint,
                cc::platform::sysconfig::TimeZoneInfoRequest *proto) noexcept
    {
        proto->set_canonical_zone(canonical_zone);
        encode(timepoint, proto->mutable_time());
    }

    void decode(const cc::platform::sysconfig::TimeZoneInfoRequest &proto,
                platform::sysconfig::TimeZoneCanonicalName *canonical_zone,
                core::dt::TimePoint *timepoint) noexcept
    {
        *canonical_zone = proto.canonical_zone();
        decode(proto.time(), timepoint);
    }


    //==========================================================================
    // Time Synchronization

    void encode(platform::sysconfig::TimeSync native,
                cc::platform::sysconfig::TimeSync *proto) noexcept
    {
        *proto = static_cast<cc::platform::sysconfig::TimeSync>(
            native - platform::sysconfig::TSYNC_NONE + cc::platform::sysconfig::TSYNC_NONE);
    }

    void decode(cc::platform::sysconfig::TimeSync proto,
                platform::sysconfig::TimeSync *native) noexcept
    {
        *native = static_cast<platform::sysconfig::TimeSync>(
            proto - cc::platform::sysconfig::TSYNC_NONE + platform::sysconfig::TSYNC_NONE);
    }

    //==========================================================================
    // TimeConfig

    void encode(const platform::sysconfig::TimeConfig &native,
                cc::platform::sysconfig::TimeConfig *proto) noexcept
    {
        proto->set_synchronization(
            encoded<cc::platform::sysconfig::TimeSync>(native.synchronization));

        for (const std::string &name : native.servers)
        {
            proto->add_servers(name);
        }
    }

    void decode(const cc::platform::sysconfig::TimeConfig &proto,
                platform::sysconfig::TimeConfig *native) noexcept
    {
        decode(proto.synchronization(), &native->synchronization);
        for (const std::string &name : proto.servers())
        {
            native->servers.push_back(name);
        }
    }

    //==========================================================================
    // CommandInvocation

    void encode(const platform::sysconfig::CommandInvocation &native,
                cc::platform::sysconfig::CommandInvocation *proto) noexcept
    {
        proto->set_working_directory(native.working_directory.string());
        for (const auto &arg : native.argv)
        {
            proto->add_argv(arg);
        }
    }

    void decode(const cc::platform::sysconfig::CommandInvocation &proto,
                platform::sysconfig::CommandInvocation *native) noexcept
    {
        native->working_directory = fs::path(proto.working_directory());
        native->argv.clear();
        for (const auto &arg : proto.argv())
        {
            native->argv.push_back(arg);
        }
    }

    //==========================================================================
    // CommandInvocationResponse

    void encode(const platform::sysconfig::CommandInvocationResponse &native,
                cc::platform::sysconfig::CommandInvocationResponse *proto) noexcept
    {
        proto->set_pid(native.pid);
    }

    void decode(const cc::platform::sysconfig::CommandInvocationResponse &proto,
                platform::sysconfig::CommandInvocationResponse *native) noexcept
    {
        native->pid = proto.pid();
    }

    //==========================================================================
    // CommandContinuation

    void encode(const platform::sysconfig::CommandContinuation &native,
                cc::platform::sysconfig::CommandContinuation *proto) noexcept
    {
        proto->set_pid(native.pid);
        proto->set_stdin(native.stdin);
    }

    void decode(const cc::platform::sysconfig::CommandContinuation &proto,
                platform::sysconfig::CommandContinuation *native) noexcept
    {
        native->pid = proto.pid();
        native->stdin = proto.stdin();
    }

    //==========================================================================
    // CommandResponse

    void encode(const platform::sysconfig::CommandResponse &native,
                cc::platform::sysconfig::CommandResponse *proto) noexcept
    {
        proto->set_stdout(native.stdout);
        proto->set_stderr(native.stderr);
        proto->set_exit_status(native.exit_status);
    }

    void decode(const cc::platform::sysconfig::CommandResponse &proto,
                platform::sysconfig::CommandResponse *native) noexcept
    {
        native->stdout = proto.stdout();
        native->stderr = proto.stderr();
        native->exit_status = proto.exit_status();
    }
}  // namespace protobuf
