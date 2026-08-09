/// -*- c++ -*-
//==============================================================================
/// @file protobuf-sysconfig-types.h++
/// @brief Encode/decode routines for types in `sysconfig.proto`
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/sysconfig/protobuf/sysconfig_types.pb.h"  // generated from `sysconfig.proto`
#include "sysconfig-host.h++"
#include "sysconfig-process.h++"
#include "sysconfig-product.h++"
#include "sysconfig-time.h++"
#include "sysconfig-timezone.h++"
#include "chrono/date-time.h++"

namespace cc::protobuf
{
    //==========================================================================
    // HostInfo

    void encode(const platform::sysconfig::HostInfo& native,
                platform::sysconfig::protobuf::HostInfo* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::HostInfo& proto,
                platform::sysconfig::HostInfo* native) noexcept;

    //==========================================================================
    // ProductInfo

    void encode(const platform::sysconfig::ProductInfo& native,
                platform::sysconfig::protobuf::ProductInfo* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::ProductInfo& proto,
                platform::sysconfig::ProductInfo* native) noexcept;

    //==========================================================================
    // SubsystemInfo

    void encode(const std::vector<platform::sysconfig::ProductInfo>& native,
                platform::sysconfig::protobuf::SubsystemInfo* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::SubsystemInfo& proto,
                std::vector<platform::sysconfig::ProductInfo>* native) noexcept;

    //==========================================================================
    // Version

    void encode(const platform::sysconfig::Version& native,
                cc::protobuf::version::Version* proto) noexcept;

    void decode(const cc::protobuf::version::Version& proto,
                platform::sysconfig::Version* native) noexcept;

    //==========================================================================
    // ComponentVersions

    void encode(const platform::sysconfig::ComponentVersions& native,
                cc::protobuf::version::ComponentVersions* proto) noexcept;

    void decode(const cc::protobuf::version::ComponentVersions& proto,
                platform::sysconfig::ComponentVersions* native) noexcept;

    //==========================================================================
    // TimeZoneCanonicalName

    void encode(const platform::sysconfig::TimeZoneCanonicalName& native,
                platform::sysconfig::protobuf::TimeZoneCanonicalName* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::TimeZoneCanonicalName& proto,
                platform::sysconfig::TimeZoneCanonicalName* native) noexcept;

    //==========================================================================
    // TimeZoneCanonicalSpec

    void encode(const platform::sysconfig::TimeZoneCanonicalSpec& native,
                platform::sysconfig::protobuf::TimeZoneCanonicalSpec* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::TimeZoneCanonicalSpec& proto,
                platform::sysconfig::TimeZoneCanonicalSpec* native) noexcept;

    //==========================================================================
    // TimeLocationFilter

    void encode(const platform::sysconfig::TimeZoneLocationFilter& native,
                platform::sysconfig::protobuf::TimeZoneLocationFilter* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::TimeZoneLocationFilter& proto,
                platform::sysconfig::TimeZoneLocationFilter* native) noexcept;

    //==========================================================================
    // TimeZoneArea

    void encode(const platform::sysconfig::TimeZoneArea& native,
                platform::sysconfig::protobuf::TimeZoneArea* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::TimeZoneArea& proto,
                platform::sysconfig::TimeZoneArea* native) noexcept;

    //==========================================================================
    // TimeZoneCountry

    void encode(const platform::sysconfig::TimeZoneCountry& native,
                platform::sysconfig::protobuf::TimeZoneCountry* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::TimeZoneCountry& proto,
                platform::sysconfig::TimeZoneCountry* native) noexcept;

    //==========================================================================
    // TimeZoneLocation

    void encode(const platform::sysconfig::TimeZoneLocation& native,
                platform::sysconfig::protobuf::TimeZoneLocation* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::TimeZoneLocation& proto,
                platform::sysconfig::TimeZoneLocation* native) noexcept;

    //==========================================================================
    // TimeZoneInfo

    void encode(const core::dt::TimeZoneInfo& native,
                platform::sysconfig::protobuf::TimeZoneInfo* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::TimeZoneInfo& proto,
                core::dt::TimeZoneInfo* native) noexcept;

    //==========================================================================
    // TimeZoneInfoRequest

    void encode(const platform::sysconfig::TimeZoneCanonicalName& canonical_zone,
                const core::dt::TimePoint& timepoint,
                platform::sysconfig::protobuf::TimeZoneInfoRequest* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::TimeZoneInfoRequest& proto,
                platform::sysconfig::TimeZoneCanonicalName* canonical_zone,
                core::dt::TimePoint* timepoint) noexcept;

    //==========================================================================
    // TimeSync

    void encode(platform::sysconfig::TimeSync native,
                platform::sysconfig::protobuf::TimeSync* proto) noexcept;

    void decode(platform::sysconfig::protobuf::TimeSync proto,
                platform::sysconfig::TimeSync* native) noexcept;

    //==========================================================================
    // TimeConfig

    void encode(const platform::sysconfig::TimeConfig& native,
                platform::sysconfig::protobuf::TimeConfig* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::TimeConfig& proto,
                platform::sysconfig::TimeConfig* native) noexcept;

    //==========================================================================
    // CommandInvocation

    void encode(const core::platform::Invocation& invocation,
                const std::string& input,
                platform::sysconfig::protobuf::CommandInvocation* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::CommandInvocation& proto,
                core::platform::Invocation* invocation,
                std::string* input = nullptr) noexcept;

    //==========================================================================
    // CommandInvocationResponse

    void encode(const core::platform::PID& native,
                platform::sysconfig::protobuf::CommandInvocationResponse* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::CommandInvocationResponse& proto,
                core::platform::PID* native) noexcept;

    //==========================================================================
    // CommandContinuation

    void encode(const core::platform::PID& pid,
                const std::string& input,
                platform::sysconfig::protobuf::CommandContinuation* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::CommandContinuation& proto,
                core::platform::PID* pid,
                std::string* input = nullptr) noexcept;

    //==========================================================================
    // CommandResponse

    void encode(const core::platform::InvocationResult& native,
                platform::sysconfig::protobuf::CommandResponse* proto) noexcept;

    void decode(const platform::sysconfig::protobuf::CommandResponse& proto,
                core::platform::InvocationResult* native) noexcept;

}  // namespace cc::protobuf
