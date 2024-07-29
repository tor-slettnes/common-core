// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-time.c++
/// @brief SysConfig service - Time configuration gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-grpc-time.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace platform::sysconfig::grpc
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
            cc::platform::sysconfig::Signal::kTime,
            [&](const cc::platform::sysconfig::Signal &signal) {
                platform::sysconfig::signal_time.emit(
                    protobuf::decoded<core::dt::TimePoint>(signal.time()));
            });

        this->client->add_handler(
            cc::platform::sysconfig::Signal::kTimeConfig,
            [&](const cc::platform::sysconfig::Signal &signal) {
                platform::sysconfig::signal_timeconfig.emit(
                    protobuf::decoded<TimeConfig>(signal.time_config()));
            });
    }

    void TimeConfigProvider::set_current_time(const core::dt::TimePoint &tp)
    {
        this->client->call_check(
            &Client::Stub::set_current_time,
            protobuf::encoded<google::protobuf::Timestamp>(tp));
    }

    core::dt::TimePoint TimeConfigProvider::get_current_time() const
    {
        return protobuf::decoded<core::dt::TimePoint>(
            this->client->call_check(
                &Client::Stub::get_current_time));
    }

    void TimeConfigProvider::set_time_config(const TimeConfig &config)
    {
        this->client->call_check(
            &Client::Stub::set_time_config,
            protobuf::encoded<cc::platform::sysconfig::TimeConfig>(config));
    }

    TimeConfig TimeConfigProvider::get_time_config() const
    {
        return protobuf::decoded<TimeConfig>(
            this->client->call_check(
                &Client::Stub::get_time_config));
    }

}  // namespace platform::sysconfig::grpc
