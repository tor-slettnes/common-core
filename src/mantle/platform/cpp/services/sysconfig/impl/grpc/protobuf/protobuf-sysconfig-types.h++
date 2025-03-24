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

    void encode(const sysconfig::HostInfo &native,
                ::cc::sysconfig::HostInfo *proto) noexcept;

    void decode(const ::cc::sysconfig::HostInfo &proto,
                sysconfig::HostInfo *native) noexcept;

    //==========================================================================
    // ProductInfo

    void encode(const sysconfig::ProductInfo &native,
                ::cc::sysconfig::ProductInfo *proto) noexcept;

    void decode(const ::cc::sysconfig::ProductInfo &proto,
                sysconfig::ProductInfo *native) noexcept;

    //==========================================================================
    // SubsystemInfo

    void encode(const std::vector<sysconfig::ProductInfo> &native,
                ::cc::sysconfig::SubsystemInfo *proto) noexcept;

    void decode(const ::cc::sysconfig::SubsystemInfo &proto,
                std::vector<sysconfig::ProductInfo> *native) noexcept;

    //==========================================================================
    // Version

    void encode(const sysconfig::Version &native,
                cc::version::Version *proto) noexcept;

    void decode(const cc::version::Version &proto,
                sysconfig::Version *native) noexcept;

    //==========================================================================
    // ComponentVersions

    void encode(const sysconfig::ComponentVersions &native,
                cc::version::ComponentVersions *proto) noexcept;

    void decode(const cc::version::ComponentVersions &proto,
                sysconfig::ComponentVersions *native) noexcept;

    //==========================================================================
    // TimeZoneCanonicalName

    void encode(const sysconfig::TimeZoneCanonicalName &native,
                ::cc::sysconfig::TimeZoneCanonicalName *proto) noexcept;

    void decode(const ::cc::sysconfig::TimeZoneCanonicalName &proto,
                sysconfig::TimeZoneCanonicalName *native) noexcept;

    //==========================================================================
    // TimeZoneCanonicalSpec

    void encode(const sysconfig::TimeZoneCanonicalSpec &native,
                ::cc::sysconfig::TimeZoneCanonicalSpec *proto) noexcept;

    void decode(const ::cc::sysconfig::TimeZoneCanonicalSpec &proto,
                sysconfig::TimeZoneCanonicalSpec *native) noexcept;

    //==========================================================================
    // TimeLocationFilter

    void encode(const sysconfig::TimeZoneLocationFilter &native,
                ::cc::sysconfig::TimeZoneLocationFilter *proto) noexcept;

    void decode(const ::cc::sysconfig::TimeZoneLocationFilter &proto,
                sysconfig::TimeZoneLocationFilter *native) noexcept;

    //==========================================================================
    // TimeZoneArea

    void encode(const sysconfig::TimeZoneArea &native,
                ::cc::sysconfig::TimeZoneArea *proto) noexcept;

    void decode(const ::cc::sysconfig::TimeZoneArea &proto,
                sysconfig::TimeZoneArea *native) noexcept;

    //==========================================================================
    // TimeZoneCountry

    void encode(const sysconfig::TimeZoneCountry &native,
                ::cc::sysconfig::TimeZoneCountry *proto) noexcept;

    void decode(const ::cc::sysconfig::TimeZoneCountry &proto,
                sysconfig::TimeZoneCountry *native) noexcept;

    //==========================================================================
    // TimeZoneLocation

    void encode(const sysconfig::TimeZoneLocation &native,
                ::cc::sysconfig::TimeZoneLocation *proto) noexcept;

    void decode(const ::cc::sysconfig::TimeZoneLocation &proto,
                sysconfig::TimeZoneLocation *native) noexcept;

    //==========================================================================
    // TimeZoneInfo

    void encode(const core::dt::TimeZoneInfo &native,
                ::cc::sysconfig::TimeZoneInfo *proto) noexcept;

    void decode(const ::cc::sysconfig::TimeZoneInfo &proto,
                core::dt::TimeZoneInfo *native) noexcept;

    //==========================================================================
    // TimeZoneInfoRequest

    void encode(const sysconfig::TimeZoneCanonicalName &canonical_zone,
                const core::dt::TimePoint &timepoint,
                ::cc::sysconfig::TimeZoneInfoRequest *proto) noexcept;

    void decode(const ::cc::sysconfig::TimeZoneInfoRequest &proto,
                sysconfig::TimeZoneCanonicalName *canonical_zone,
                core::dt::TimePoint *timepoint) noexcept;

    //==========================================================================
    // TimeSync

    void encode(sysconfig::TimeSync native,
                ::cc::sysconfig::TimeSync *proto) noexcept;

    void decode(::cc::sysconfig::TimeSync proto,
                sysconfig::TimeSync *native) noexcept;

    //==========================================================================
    // TimeConfig

    void encode(const sysconfig::TimeConfig &native,
                ::cc::sysconfig::TimeConfig *proto) noexcept;

    void decode(const ::cc::sysconfig::TimeConfig &proto,
                sysconfig::TimeConfig *native) noexcept;

    //==========================================================================
    // CommandInvocation

    void encode(const core::platform::Invocation &invocation,
                const std::string &input,
                ::cc::sysconfig::CommandInvocation *proto) noexcept;

    void decode(const ::cc::sysconfig::CommandInvocation &proto,
                core::platform::Invocation *invocation,
                std::string *input = nullptr) noexcept;

    //==========================================================================
    // CommandInvocationResponse

    void encode(const core::platform::PID &native,
                ::cc::sysconfig::CommandInvocationResponse *proto) noexcept;

    void decode(const ::cc::sysconfig::CommandInvocationResponse &proto,
                core::platform::PID *native) noexcept;

    //==========================================================================
    // CommandContinuation

    void encode(const core::platform::PID &pid,
                const std::string &input,
                ::cc::sysconfig::CommandContinuation *proto) noexcept;

    void decode(const ::cc::sysconfig::CommandContinuation &proto,
                core::platform::PID *pid,
                std::string *input = nullptr) noexcept;

    //==========================================================================
    // CommandResponse

    void encode(const core::platform::InvocationResult &native,
                ::cc::sysconfig::CommandResponse *proto) noexcept;

    void decode(const ::cc::sysconfig::CommandResponse &proto,
                core::platform::InvocationResult *native) noexcept;

}  // namespace protobuf
