// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-time.c++
/// @brief SysConfig gRPC implementation - time/date settings
/// @author Tor Slettnes
//==============================================================================

#include "sysconfig-grpc-time.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace cc::platform::sysconfig::grpc
{
    TimeConfigProvider::TimeConfigProvider(const std::shared_ptr<Client> &client)
        : Super(TYPE_NAME_FULL(This)),
          client(client)
    {
    }

    void TimeConfigProvider::initialize()
    {
        Super::initialize();
        this->client->add_handler(
            platform::sysconfig::protobuf::Signal::kTime,
            [&](const platform::sysconfig::protobuf::Signal &signal) {
                sysconfig::signal_time.emit(
                    cc::protobuf::decoded<core::dt::TimePoint>(signal.time()));
            });

        this->client->add_handler(
            platform::sysconfig::protobuf::Signal::kTimeConfig,
            [&](const platform::sysconfig::protobuf::Signal &signal) {
                sysconfig::signal_timeconfig.emit(
                    cc::protobuf::decoded<TimeConfig>(signal.time_config()));
            });
    }

    void TimeConfigProvider::set_current_time(const core::dt::TimePoint &tp)
    {
        this->client->call_check(
            &Client::Stub::SetCurrentTime,
            cc::protobuf::encoded<google::protobuf::Timestamp>(tp));
    }

    core::dt::TimePoint TimeConfigProvider::get_current_time() const
    {
        return cc::protobuf::decoded<core::dt::TimePoint>(
            this->client->call_check(
                &Client::Stub::GetCurrentTime));
    }

    void TimeConfigProvider::set_time_config(const TimeConfig &config)
    {
        this->client->call_check(
            &Client::Stub::SetTimeConfig,
            cc::protobuf::encoded<platform::sysconfig::protobuf::TimeConfig>(config));
    }

    TimeConfig TimeConfigProvider::get_time_config() const
    {
        return cc::protobuf::decoded<TimeConfig>(
            this->client->call_check(
                &Client::Stub::GetTimeConfig));
    }

}  // namespace cc::platform::sysconfig::grpc
