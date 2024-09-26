/// -*- c++ -*-
//==============================================================================
/// @file protobuf-sysconfig-types.h++
/// @brief Encode/decode routines for types in `sysconfig.proto`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig.pb.h"  // generated from `sysconfig.proto`
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

    void encode(const platform::sysconfig::HostInfo &native,
                cc::platform::sysconfig::HostInfo *proto) noexcept;

    void decode(const cc::platform::sysconfig::HostInfo &proto,
                platform::sysconfig::HostInfo *native) noexcept;

    //==========================================================================
    // ProductInfo

    void encode(const platform::sysconfig::ProductInfo &native,
                cc::platform::sysconfig::ProductInfo *proto) noexcept;

    void decode(const cc::platform::sysconfig::ProductInfo &proto,
                platform::sysconfig::ProductInfo *native) noexcept;

    //==========================================================================
    // SubsystemInfo

    void encode(const std::vector<platform::sysconfig::ProductInfo> &native,
                cc::platform::sysconfig::SubsystemInfo *proto) noexcept;

    void decode(const cc::platform::sysconfig::SubsystemInfo &proto,
                std::vector<platform::sysconfig::ProductInfo> *native) noexcept;

    //==========================================================================
    // Version

    void encode(const platform::sysconfig::Version &native,
                cc::version::Version *proto) noexcept;

    void decode(const cc::version::Version &proto,
                platform::sysconfig::Version *native) noexcept;

    //==========================================================================
    // ComponentVersions

    void encode(const platform::sysconfig::ComponentVersions &native,
                cc::version::ComponentVersions *proto) noexcept;

    void decode(const cc::version::ComponentVersions &proto,
                platform::sysconfig::ComponentVersions *native) noexcept;

    //==========================================================================
    // TimeZoneCanonicalName

    void encode(const platform::sysconfig::TimeZoneCanonicalName &native,
                cc::platform::sysconfig::TimeZoneCanonicalName *proto) noexcept;

    void decode(const cc::platform::sysconfig::TimeZoneCanonicalName &proto,
                platform::sysconfig::TimeZoneCanonicalName *native) noexcept;

    //==========================================================================
    // TimeZoneCanonicalSpec

    void encode(const platform::sysconfig::TimeZoneCanonicalSpec &native,
                cc::platform::sysconfig::TimeZoneCanonicalSpec *proto) noexcept;

    void decode(const cc::platform::sysconfig::TimeZoneCanonicalSpec &proto,
                platform::sysconfig::TimeZoneCanonicalSpec *native) noexcept;

    //==========================================================================
    // TimeLocationFilter

    void encode(const platform::sysconfig::TimeZoneLocationFilter &native,
                cc::platform::sysconfig::TimeZoneLocationFilter *proto) noexcept;

    void decode(const cc::platform::sysconfig::TimeZoneLocationFilter &proto,
                platform::sysconfig::TimeZoneLocationFilter *native) noexcept;

    //==========================================================================
    // TimeZoneArea

    void encode(const platform::sysconfig::TimeZoneArea &native,
                cc::platform::sysconfig::TimeZoneArea *proto) noexcept;

    void decode(const cc::platform::sysconfig::TimeZoneArea &proto,
                platform::sysconfig::TimeZoneArea *native) noexcept;

    //==========================================================================
    // TimeZoneCountry

    void encode(const platform::sysconfig::TimeZoneCountry &native,
                cc::platform::sysconfig::TimeZoneCountry *proto) noexcept;

    void decode(const cc::platform::sysconfig::TimeZoneCountry &proto,
                platform::sysconfig::TimeZoneCountry *native) noexcept;

    //==========================================================================
    // TimeZoneLocation

    void encode(const platform::sysconfig::TimeZoneLocation &native,
                cc::platform::sysconfig::TimeZoneLocation *proto) noexcept;

    void decode(const cc::platform::sysconfig::TimeZoneLocation &proto,
                platform::sysconfig::TimeZoneLocation *native) noexcept;

    //==========================================================================
    // TimeZoneInfo

    void encode(const core::dt::TimeZoneInfo &native,
                cc::platform::sysconfig::TimeZoneInfo *proto) noexcept;

    void decode(const cc::platform::sysconfig::TimeZoneInfo &proto,
                core::dt::TimeZoneInfo *native) noexcept;

    //==========================================================================
    // TimeZoneInfoRequest

    void encode(const platform::sysconfig::TimeZoneCanonicalName &canonical_zone,
                const core::dt::TimePoint &timepoint,
                cc::platform::sysconfig::TimeZoneInfoRequest *proto) noexcept;

    void decode(const cc::platform::sysconfig::TimeZoneInfoRequest &proto,
                platform::sysconfig::TimeZoneCanonicalName *canonical_zone,
                core::dt::TimePoint *timepoint) noexcept;

    //==========================================================================
    // TimeSync

    void encode(platform::sysconfig::TimeSync native,
                cc::platform::sysconfig::TimeSync *proto) noexcept;

    void decode(cc::platform::sysconfig::TimeSync proto,
                platform::sysconfig::TimeSync *native) noexcept;

    //==========================================================================
    // TimeConfig

    void encode(const platform::sysconfig::TimeConfig &native,
                cc::platform::sysconfig::TimeConfig *proto) noexcept;

    void decode(const cc::platform::sysconfig::TimeConfig &proto,
                platform::sysconfig::TimeConfig *native) noexcept;

    //==========================================================================
    // CommandInvocation

    void encode(const core::platform::Invocation &invocation,
                const std::string &input,
                cc::platform::sysconfig::CommandInvocation *proto) noexcept;

    void decode(const cc::platform::sysconfig::CommandInvocation &proto,
                core::platform::Invocation *invocation,
                std::string *input = nullptr) noexcept;

    //==========================================================================
    // CommandInvocationResponse

    void encode(const core::platform::PID &native,
                cc::platform::sysconfig::CommandInvocationResponse *proto) noexcept;

    void decode(const cc::platform::sysconfig::CommandInvocationResponse &proto,
                core::platform::PID *native) noexcept;

    //==========================================================================
    // CommandContinuation

    void encode(const core::platform::PID &pid,
                const std::string &input,
                cc::platform::sysconfig::CommandContinuation *proto) noexcept;

    void decode(const cc::platform::sysconfig::CommandContinuation &proto,
                core::platform::PID *pid,
                std::string *input = nullptr) noexcept;

    //==========================================================================
    // CommandResponse

    void encode(const core::platform::InvocationResult &native,
                cc::platform::sysconfig::CommandResponse *proto) noexcept;

    void decode(const cc::platform::sysconfig::CommandResponse &proto,
                core::platform::InvocationResult *native) noexcept;

}  // namespace protobuf
