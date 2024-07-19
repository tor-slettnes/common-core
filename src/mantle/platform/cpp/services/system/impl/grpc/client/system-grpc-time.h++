// -*- c++ -*-
//==============================================================================
/// @file system-grpc-time.h++
/// @brief System service - Time configuration gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "system-provider-time.h++"
#include "system-grpc-client.h++"

namespace platform::system::grpc
{
    class Time : public TimeProvider
    {
        using Super = TimeProvider;

    public:
        Time(const std::shared_ptr<Client> &client);

        void initialize() override;

    protected:
        // Current timestamp
        void set_current_time(const core::dt::TimePoint &tp) override;
        core::dt::TimePoint get_current_time() const override;

        // Get or set time configuration
        void set_time_config(const TimeConfig &config) override;
        TimeConfig get_time_config() const override;

    private:
        std::shared_ptr<Client> client;
    };
}  // namespace platform::system::grpc
