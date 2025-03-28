/// -*- c++ -*-
//==============================================================================
/// @file protobuf-sysconfig-types.c++
/// @brief Encode/decode routines for types in `sysconfig.proto`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-sysconfig-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-message.h++"
#include "protobuf-inline.h++"

namespace protobuf
{

    //==========================================================================
    // HostInfo

    void encode(const sysconfig::HostInfo &native,
                ::cc::platform::sysconfig::HostInfo *proto) noexcept
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

    void decode(const ::cc::platform::sysconfig::HostInfo &proto,
                sysconfig::HostInfo *native) noexcept
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

    void encode(const sysconfig::ProductInfo &native,
                ::cc::platform::sysconfig::ProductInfo *proto) noexcept
    {
        proto->set_product_name(native.product_name);
        proto->set_product_description(native.product_description);
        proto->set_product_serial(native.product_serial);
        proto->set_hardware_model(native.hardware_model);
        encode(native.release_version, proto->mutable_release_version());
        encode(native.component_versions, proto->mutable_component_versions());
        encode(native.subsystem_info, proto->mutable_subsystem_info());
    }

    void decode(const ::cc::platform::sysconfig::ProductInfo &proto,
                sysconfig::ProductInfo *native) noexcept
    {
        native->product_name = proto.product_name();
        native->product_description = proto.product_description();
        native->product_serial = proto.product_serial();
        native->hardware_model = proto.hardware_model();
        decode(proto.release_version(), &native->release_version);
        decode(proto.component_versions(), &native->component_versions);
        decode(proto.subsystem_info(), &native->subsystem_info);
    }

    //==========================================================================
    // SubsystemInfo

    void encode(const std::vector<sysconfig::ProductInfo> &native,
                ::cc::platform::sysconfig::SubsystemInfo *proto) noexcept
    {
        encode_vector(native, proto->mutable_subsystems());
    }

    void decode(const ::cc::platform::sysconfig::SubsystemInfo &proto,
                std::vector<sysconfig::ProductInfo> *native) noexcept
    {
        decode_to_vector(proto.subsystems(), native);
    }

    //==========================================================================
    // Version

    void encode(const sysconfig::Version &native,
                cc::version::Version *proto) noexcept
    {
        proto->set_major(native.major);
        proto->set_minor(native.minor);
        proto->set_patch(native.patch);
        proto->set_printable_version(native.printable_version);
        proto->set_build_number(native.build_number);
    }

    void decode(const cc::version::Version &proto,
                sysconfig::Version *native) noexcept
    {
        native->major = proto.major();
        native->minor = proto.minor();
        native->patch = proto.patch();
        native->printable_version = proto.printable_version();
        native->build_number = proto.build_number();
    }

    //==========================================================================
    // ComponentVersions

    void encode(const sysconfig::ComponentVersions &native,
                cc::version::ComponentVersions *proto) noexcept
    {
        google::protobuf::Map<std::string, cc::version::Version> *cv = proto->mutable_components();

        for (const auto &[component, version] : native)
        {
            encode(version, &(*cv)[component]);
        }
    }

    void decode(const cc::version::ComponentVersions &proto,
                sysconfig::ComponentVersions *native) noexcept
    {
        for (const auto &[component, version] : proto.components())
        {
            decode(version, &(*native)[component]);
        }
    }

    //==========================================================================
    // TimeZoneCanonicalName

    void encode(const sysconfig::TimeZoneCanonicalName &native,
                ::cc::platform::sysconfig::TimeZoneCanonicalName *proto) noexcept
    {
        proto->set_name(native);
    }

    void decode(const ::cc::platform::sysconfig::TimeZoneCanonicalName &proto,
                sysconfig::TimeZoneCanonicalName *native) noexcept
    {
        *native = proto.name();
    }

    //==========================================================================
    // TimeZoneCanonicalSpec

    void encode(const sysconfig::TimeZoneCanonicalSpec &native,
                ::cc::platform::sysconfig::TimeZoneCanonicalSpec *proto) noexcept
    {
        proto->set_name(native.name);
        proto->set_area(native.area);

        encode_vector<::cc::platform::sysconfig::TimeZoneLocation>(
            native.locations,
            proto->mutable_locations());

        proto->set_latitude(native.latitude);
        proto->set_longitude(native.longitude);
    }

    void decode(const ::cc::platform::sysconfig::TimeZoneCanonicalSpec &proto,
                sysconfig::TimeZoneCanonicalSpec *native) noexcept
    {
        native->name = proto.name();
        native->area = proto.area();
        decode_to_vector<sysconfig::TimeZoneLocation>(
            proto.locations(),
            &native->locations);
        native->latitude = proto.latitude();
        native->longitude = proto.longitude();
    }

    //==========================================================================
    // TimeLocationFilter

    void encode(const sysconfig::TimeZoneLocationFilter &native,
                ::cc::platform::sysconfig::TimeZoneLocationFilter *proto) noexcept
    {
        encode(native.area, proto->mutable_area());
        encode(native.country, proto->mutable_country());
    }

    void decode(const ::cc::platform::sysconfig::TimeZoneLocationFilter &proto,
                sysconfig::TimeZoneLocationFilter *native) noexcept
    {
        decode(proto.area(), &native->area);
        decode(proto.country(), &native->country);
    }

    //==========================================================================
    // TimeZoneArea

    void encode(const sysconfig::TimeZoneArea &native,
                ::cc::platform::sysconfig::TimeZoneArea *proto) noexcept
    {
        proto->set_name(native);
    }

    void decode(const ::cc::platform::sysconfig::TimeZoneArea &proto,
                sysconfig::TimeZoneArea *native) noexcept
    {
        *native = proto.name();
    }

    //==========================================================================
    // TimeZoneCountry

    void encode(const sysconfig::TimeZoneCountry &native,
                ::cc::platform::sysconfig::TimeZoneCountry *proto) noexcept
    {
        proto->set_code(native.code);
        proto->set_name(native.name);
    }

    void decode(const ::cc::platform::sysconfig::TimeZoneCountry &proto,
                sysconfig::TimeZoneCountry *native) noexcept
    {
        native->code = proto.code();
        native->name = proto.name();
    }

    //==========================================================================
    // TimeZoneLocation

    void encode(const sysconfig::TimeZoneLocation &native,
                ::cc::platform::sysconfig::TimeZoneLocation *proto) noexcept
    {
        encode(native.country, proto->mutable_country());
        proto->set_region(native.region);
    }

    void decode(const ::cc::platform::sysconfig::TimeZoneLocation &proto,
                sysconfig::TimeZoneLocation *native) noexcept
    {
        decode(proto.country(), &native->country);
        native->region = proto.region();
    }

    //==========================================================================
    // TimeZoneInfo

    void encode(const core::dt::TimeZoneInfo &native,
                ::cc::platform::sysconfig::TimeZoneInfo *proto) noexcept
    {
        proto->set_shortname(native.shortname);
        encode(native.offset, proto->mutable_offset());
        encode(native.stdoffset, proto->mutable_stdoffset());
        proto->set_dst(native.dst);
    }

    void decode(const ::cc::platform::sysconfig::TimeZoneInfo &proto,
                core::dt::TimeZoneInfo *native) noexcept
    {
        native->shortname = proto.shortname();
        decode(proto.offset(), &native->offset);
        decode(proto.stdoffset(), &native->stdoffset);
        native->dst = proto.dst();
    }

    //==========================================================================
    // TimeZoneInfoRequest

    void encode(const sysconfig::TimeZoneCanonicalName &canonical_zone,
                const core::dt::TimePoint &timepoint,
                ::cc::platform::sysconfig::TimeZoneInfoRequest *proto) noexcept
    {
        proto->set_canonical_zone(canonical_zone);
        encode(timepoint, proto->mutable_time());
    }

    void decode(const ::cc::platform::sysconfig::TimeZoneInfoRequest &proto,
                sysconfig::TimeZoneCanonicalName *canonical_zone,
                core::dt::TimePoint *timepoint) noexcept
    {
        *canonical_zone = proto.canonical_zone();
        decode(proto.time(), timepoint);
    }

    //==========================================================================
    // Time Synchronization

    void encode(sysconfig::TimeSync native,
                ::cc::platform::sysconfig::TimeSync *proto) noexcept
    {
        *proto = static_cast<::cc::platform::sysconfig::TimeSync>(
            native - sysconfig::TSYNC_NONE + ::cc::platform::sysconfig::TSYNC_NONE);
    }

    void decode(::cc::platform::sysconfig::TimeSync proto,
                sysconfig::TimeSync *native) noexcept
    {
        *native = static_cast<sysconfig::TimeSync>(
            proto - ::cc::platform::sysconfig::TSYNC_NONE + sysconfig::TSYNC_NONE);
    }

    //==========================================================================
    // TimeConfig

    void encode(const sysconfig::TimeConfig &native,
                ::cc::platform::sysconfig::TimeConfig *proto) noexcept
    {
        proto->set_synchronization(
            encoded<::cc::platform::sysconfig::TimeSync>(native.synchronization));

        for (const std::string &name : native.servers)
        {
            proto->add_servers(name);
        }
    }

    void decode(const ::cc::platform::sysconfig::TimeConfig &proto,
                sysconfig::TimeConfig *native) noexcept
    {
        decode(proto.synchronization(), &native->synchronization);
        for (const std::string &name : proto.servers())
        {
            native->servers.push_back(name);
        }
    }

    //==========================================================================
    // CommandInvocation

    void encode(const core::platform::Invocation &invocation,
                const std::string &input,
                ::cc::platform::sysconfig::CommandInvocation *proto) noexcept
    {
        assign_repeated(invocation.argv, proto->mutable_argv());
        proto->set_working_directory(invocation.cwd);
        proto->set_stdin(input);
    }

    void decode(const ::cc::platform::sysconfig::CommandInvocation &proto,
                core::platform::Invocation *invocation,
                std::string *input) noexcept
    {
        assign_to_vector(proto.argv(), &invocation->argv);
        invocation->cwd = proto.working_directory();
        if (input)
        {
            *input = proto.stdin();
        }
    }

    //==========================================================================
    // CommandInvocationResponse

    void encode(const core::platform::PID &native,
                ::cc::platform::sysconfig::CommandInvocationResponse *proto) noexcept
    {
        proto->set_pid(native);
    }

    void decode(const ::cc::platform::sysconfig::CommandInvocationResponse &proto,
                core::platform::PID *native) noexcept
    {
        *native = proto.pid();
    }

    //==========================================================================
    // CommandContinuation

    void encode(const core::platform::PID &pid,
                const std::string &input,
                ::cc::platform::sysconfig::CommandContinuation *proto) noexcept
    {
        proto->set_pid(pid);
        proto->set_stdin(input);
    }

    void decode(const ::cc::platform::sysconfig::CommandContinuation &proto,
                core::platform::PID *pid,
                std::string *input) noexcept
    {
        if (pid)
        {
            *pid = proto.pid();
        }

        if (input)
        {
            *input = proto.stdin();
        }
    }

    //==========================================================================
    // CommandResponse

    void encode(const core::platform::InvocationResult &native,
                ::cc::platform::sysconfig::CommandResponse *proto) noexcept
    {
        if (native.stdout)
        {
            proto->set_stdout(native.stdout->str());
        }

        if (native.stderr)
        {
            proto->set_stderr(native.stderr->str());
        }

        if (native.status)
        {
            proto->set_success(native.status->success());

            if (int code = native.status->exit_code())
            {
                proto->set_exit_code(code);
            }
            else if (int signal = native.status->exit_signal())
            {
                proto->set_exit_signal(signal);
            }

            proto->set_error_symbol(native.status->symbol());
            proto->set_error_text(native.status->text());
        }
    }

    void decode(const ::cc::platform::sysconfig::CommandResponse &proto,
                core::platform::InvocationResult *native) noexcept
    {
        native->stdout->write(proto.stdout().data(), proto.stdout().size());
        native->stderr->write(proto.stderr().data(), proto.stderr().size());
        native->status = std::make_shared<sysconfig::PortableExitStatus>(
            proto.success(),
            proto.exit_code(),
            proto.exit_signal(),
            proto.error_symbol(),
            proto.error_text());
    }
}  // namespace protobuf
