// -*- c++ -*-
//==============================================================================
/// @file system-grpc-signalqueue.c++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-grpc-signalqueue.h++"
#include "system-providers.h++"
#include "protobuf-system-types.h++"
#include "protobuf-standard-types.h++"

namespace platform::system::grpc
{
    void SignalQueue::initialize()
    {
        using cc::platform::system::Signal;

        this->connect<core::dt::TimePoint>(
            Signal::kTime,
            platform::system::signal_time,
            [](const core::dt::TimePoint &tp, Signal *msg)
            {
                core::protobuf::encode(tp, msg->mutable_time());
            });

        this->connect<TimeConfig>(
            Signal::kTimeConfig,
            platform::system::signal_timeconfig,
            [](const TimeConfig &tc, Signal *msg)
            {
                msg->mutable_time_config()->CopyFrom(tc);
            });

        this->connect<core::dt::TimeZoneInfo>(
            Signal::kTzInfo,
            platform::system::signal_tzinfo,
            [](const core::dt::TimeZoneInfo &zi, Signal *msg)
            {
                core::protobuf::encode(zi, msg->mutable_tz_info());
            });

        this->connect<TimeZoneConfig>(
            Signal::kTzConfig,
            platform::system::signal_tzconfig,
            [](const TimeZoneConfig &zc, Signal *msg)
            {
                msg->mutable_tz_config()->CopyFrom(zc);
            });

        this->connect<HostInfo>(
            Signal::kHostInfo,
            platform::system::signal_hostinfo,
            [](const HostInfo &hi, Signal *msg)
            {
                msg->mutable_host_info()->CopyFrom(hi);
            });

        this->connect<ProductInfo>(
            Signal::kProductInfo,
            platform::system::signal_productinfo,
            [](const ProductInfo &pi, Signal *msg)
            {
                msg->mutable_product_info()->CopyFrom(pi);
            });

        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(platform::system::signal_productinfo);
        this->disconnect(platform::system::signal_hostinfo);
        this->disconnect(platform::system::signal_tzconfig);
        this->disconnect(platform::system::signal_tzinfo);
        this->disconnect(platform::system::signal_timeconfig);
        this->disconnect(platform::system::signal_time);
        Super::deinitialize();
    }
}  // namespace sam::foup::grpc
