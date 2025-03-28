// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-signalqueue.c++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-grpc-signalqueue.h++"
#include "sysconfig.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-standard-types.h++"

namespace sysconfig::grpc
{
    void SignalQueue::initialize()
    {
        using ::cc::platform::sysconfig::Signal;

        this->connect<core::dt::TimePoint>(
            Signal::kTime,
            sysconfig::signal_time,
            [](const core::dt::TimePoint &tp, Signal *msg) {
                protobuf::encode(tp, msg->mutable_time());
            });

        this->connect<TimeConfig>(
            Signal::kTimeConfig,
            sysconfig::signal_timeconfig,
            [](const TimeConfig &tc, Signal *msg) {
                protobuf::encode(tc, msg->mutable_time_config());
            });

        this->connect<core::dt::TimeZoneInfo>(
            Signal::kTzInfo,
            sysconfig::signal_tzinfo,
            [](const core::dt::TimeZoneInfo &zi, Signal *msg) {
                protobuf::encode(zi, msg->mutable_tz_info());
            });

        this->connect<TimeZoneCanonicalName>(
            Signal::kTzConfig,
            sysconfig::signal_tzconfig,
            [](const TimeZoneCanonicalName &zonename, Signal *msg) {
                protobuf::encode(zonename, msg->mutable_tz_config());
            });

        this->connect<HostInfo>(
            Signal::kHostInfo,
            sysconfig::signal_hostinfo,
            [](const HostInfo &hi, Signal *msg) {
                protobuf::encode(hi, msg->mutable_host_info());
            });

        this->connect<ProductInfo>(
            Signal::kProductInfo,
            sysconfig::signal_productinfo,
            [](const ProductInfo &pi, Signal *msg) {
                protobuf::encode(pi, msg->mutable_product_info());
            });

        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(sysconfig::signal_productinfo);
        this->disconnect(sysconfig::signal_hostinfo);
        this->disconnect(sysconfig::signal_tzconfig);
        this->disconnect(sysconfig::signal_tzinfo);
        this->disconnect(sysconfig::signal_timeconfig);
        this->disconnect(sysconfig::signal_time);
        Super::deinitialize();
    }
}  // namespace sysconfig::grpc
