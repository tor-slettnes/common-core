/// -*- c++ -*-
//==============================================================================
/// @file protobuf-sysconfig-types.h++
/// @brief Encode/decode routines for types in `sysconfig.proto`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/platform/sysconfig/protobuf/sysconfig_types.pb.h"  // generated from `sysconfig.proto`
#include "sysconfig-host.h++"
#include "sysconfig-process.h++"
#include "sysconfig-product.h++"
#include "sysconfig-time.h++"
#include "sysconfig-timezone.h++"
#include "chrono/date-time.h++"

namespace protobuf
{
    //==========================================================================
    // HostInfo

    void encode(const sysconfig::HostInfo &native,
                ::cc::platform::sysconfig::protobuf::HostInfo *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::HostInfo &proto,
                sysconfig::HostInfo *native) noexcept;

    //==========================================================================
    // ProductInfo

    void encode(const sysconfig::ProductInfo &native,
                ::cc::platform::sysconfig::protobuf::ProductInfo *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::ProductInfo &proto,
                sysconfig::ProductInfo *native) noexcept;

    //==========================================================================
    // SubsystemInfo

    void encode(const std::vector<sysconfig::ProductInfo> &native,
                ::cc::platform::sysconfig::protobuf::SubsystemInfo *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::SubsystemInfo &proto,
                std::vector<sysconfig::ProductInfo> *native) noexcept;

    //==========================================================================
    // Version

    void encode(const sysconfig::Version &native,
                cc::protobuf::version::Version *proto) noexcept;

    void decode(const cc::protobuf::version::Version &proto,
                sysconfig::Version *native) noexcept;

    //==========================================================================
    // ComponentVersions

    void encode(const sysconfig::ComponentVersions &native,
                cc::protobuf::version::ComponentVersions *proto) noexcept;

    void decode(const cc::protobuf::version::ComponentVersions &proto,
                sysconfig::ComponentVersions *native) noexcept;

    //==========================================================================
    // TimeZoneCanonicalName

    void encode(const sysconfig::TimeZoneCanonicalName &native,
                ::cc::platform::sysconfig::protobuf::TimeZoneCanonicalName *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::TimeZoneCanonicalName &proto,
                sysconfig::TimeZoneCanonicalName *native) noexcept;

    //==========================================================================
    // TimeZoneCanonicalSpec

    void encode(const sysconfig::TimeZoneCanonicalSpec &native,
                ::cc::platform::sysconfig::protobuf::TimeZoneCanonicalSpec *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::TimeZoneCanonicalSpec &proto,
                sysconfig::TimeZoneCanonicalSpec *native) noexcept;

    //==========================================================================
    // TimeLocationFilter

    void encode(const sysconfig::TimeZoneLocationFilter &native,
                ::cc::platform::sysconfig::protobuf::TimeZoneLocationFilter *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::TimeZoneLocationFilter &proto,
                sysconfig::TimeZoneLocationFilter *native) noexcept;

    //==========================================================================
    // TimeZoneArea

    void encode(const sysconfig::TimeZoneArea &native,
                ::cc::platform::sysconfig::protobuf::TimeZoneArea *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::TimeZoneArea &proto,
                sysconfig::TimeZoneArea *native) noexcept;

    //==========================================================================
    // TimeZoneCountry

    void encode(const sysconfig::TimeZoneCountry &native,
                ::cc::platform::sysconfig::protobuf::TimeZoneCountry *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::TimeZoneCountry &proto,
                sysconfig::TimeZoneCountry *native) noexcept;

    //==========================================================================
    // TimeZoneLocation

    void encode(const sysconfig::TimeZoneLocation &native,
                ::cc::platform::sysconfig::protobuf::TimeZoneLocation *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::TimeZoneLocation &proto,
                sysconfig::TimeZoneLocation *native) noexcept;

    //==========================================================================
    // TimeZoneInfo

    void encode(const core::dt::TimeZoneInfo &native,
                ::cc::platform::sysconfig::protobuf::TimeZoneInfo *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::TimeZoneInfo &proto,
                core::dt::TimeZoneInfo *native) noexcept;

    //==========================================================================
    // TimeZoneInfoRequest

    void encode(const sysconfig::TimeZoneCanonicalName &canonical_zone,
                const core::dt::TimePoint &timepoint,
                ::cc::platform::sysconfig::protobuf::TimeZoneInfoRequest *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::TimeZoneInfoRequest &proto,
                sysconfig::TimeZoneCanonicalName *canonical_zone,
                core::dt::TimePoint *timepoint) noexcept;

    //==========================================================================
    // TimeSync

    void encode(sysconfig::TimeSync native,
                ::cc::platform::sysconfig::protobuf::TimeSync *proto) noexcept;

    void decode(::cc::platform::sysconfig::protobuf::TimeSync proto,
                sysconfig::TimeSync *native) noexcept;

    //==========================================================================
    // TimeConfig

    void encode(const sysconfig::TimeConfig &native,
                ::cc::platform::sysconfig::protobuf::TimeConfig *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::TimeConfig &proto,
                sysconfig::TimeConfig *native) noexcept;

    //==========================================================================
    // CommandInvocation

    void encode(const core::platform::Invocation &invocation,
                const std::string &input,
                ::cc::platform::sysconfig::protobuf::CommandInvocation *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::CommandInvocation &proto,
                core::platform::Invocation *invocation,
                std::string *input = nullptr) noexcept;

    //==========================================================================
    // CommandInvocationResponse

    void encode(const core::platform::PID &native,
                ::cc::platform::sysconfig::protobuf::CommandInvocationResponse *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::CommandInvocationResponse &proto,
                core::platform::PID *native) noexcept;

    //==========================================================================
    // CommandContinuation

    void encode(const core::platform::PID &pid,
                const std::string &input,
                ::cc::platform::sysconfig::protobuf::CommandContinuation *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::CommandContinuation &proto,
                core::platform::PID *pid,
                std::string *input = nullptr) noexcept;

    //==========================================================================
    // CommandResponse

    void encode(const core::platform::InvocationResult &native,
                ::cc::platform::sysconfig::protobuf::CommandResponse *proto) noexcept;

    void decode(const ::cc::platform::sysconfig::protobuf::CommandResponse &proto,
                core::platform::InvocationResult *native) noexcept;

}  // namespace protobuf
