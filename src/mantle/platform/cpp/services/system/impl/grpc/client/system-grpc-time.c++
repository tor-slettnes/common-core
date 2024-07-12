// -*- c++ -*-
//==============================================================================
/// @file system-grpc-time.c++
/// @brief System service - Time configuration gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-grpc-time.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace platform::system::grpc
{
    Time::Time(const std::shared_ptr<Client> &client)
        : Super("grpc::Time"),
          client(client)
    {
    }

    void Time::initialize()
    {
        Super::initialize();
        this->client->add_handler(
            cc::platform::system::Signal::kTime,
            [&](const cc::platform::system::Signal &signal) {
                platform::system::signal_time.emit(
                    core::protobuf::decoded<core::dt::TimePoint>(signal.time()));
            });

        this->client->add_handler(
            cc::platform::system::Signal::kTimeConfig,
            [&](const cc::platform::system::Signal &signal) {
                platform::system::signal_timeconfig.emit(signal.time_config());
            });
    }

    void Time::set_current_time(const core::dt::TimePoint &tp)
    {
        this->client->call_check(
            &Client::Stub::set_current_time,
            core::protobuf::encoded<google::protobuf::Timestamp>(tp));
    }

    core::dt::TimePoint Time::get_current_time() const
    {
        return core::protobuf::decoded<core::dt::TimePoint>(
            this->client->call_check(
                &Client::Stub::get_current_time));
    }

    void Time::set_time_config(const TimeConfig &config)
    {
        this->client->call_check(&Client::Stub::set_time_config, config);
    }

    TimeConfig Time::get_time_config() const
    {
        return this->client->call_check(&Client::Stub::get_time_config);
    }

}  // namespace platform::system::grpc
