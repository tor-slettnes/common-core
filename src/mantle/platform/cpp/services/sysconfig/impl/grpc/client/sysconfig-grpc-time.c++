// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-time.c++
/// @brief SysConfig gRPC implementation - time/date settings
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-grpc-time.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace sysconfig::grpc
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
            ::cc::platform::sysconfig::protobuf::Signal::kTime,
            [&](const ::cc::platform::sysconfig::protobuf::Signal &signal) {
                sysconfig::signal_time.emit(
                    protobuf::decoded<core::dt::TimePoint>(signal.time()));
            });

        this->client->add_handler(
            ::cc::platform::sysconfig::protobuf::Signal::kTimeConfig,
            [&](const ::cc::platform::sysconfig::protobuf::Signal &signal) {
                sysconfig::signal_timeconfig.emit(
                    protobuf::decoded<TimeConfig>(signal.time_config()));
            });
    }

    void TimeConfigProvider::set_current_time(const core::dt::TimePoint &tp)
    {
        this->client->call_check(
            &Client::Stub::SetCurrentTime,
            protobuf::encoded<google::protobuf::Timestamp>(tp));
    }

    core::dt::TimePoint TimeConfigProvider::get_current_time() const
    {
        return protobuf::decoded<core::dt::TimePoint>(
            this->client->call_check(
                &Client::Stub::GetCurrentTime));
    }

    void TimeConfigProvider::set_time_config(const TimeConfig &config)
    {
        this->client->call_check(
            &Client::Stub::SetTimeConfig,
            protobuf::encoded<::cc::platform::sysconfig::protobuf::TimeConfig>(config));
    }

    TimeConfig TimeConfigProvider::get_time_config() const
    {
        return protobuf::decoded<TimeConfig>(
            this->client->call_check(
                &Client::Stub::GetTimeConfig));
    }

}  // namespace sysconfig::grpc
