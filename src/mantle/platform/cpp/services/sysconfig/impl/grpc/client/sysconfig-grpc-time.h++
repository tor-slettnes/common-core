// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-time.h++
/// @brief SysConfig service - Time configuration gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-time.h++"
#include "sysconfig-grpc-client.h++"

namespace platform::sysconfig::grpc
{
    class TimeConfigProvider : public TimeConfigInterface
    {
        using This = TimeConfigProvider;
        using Super = TimeConfigInterface;

    public:
        TimeConfigProvider(const std::shared_ptr<Client> &client);

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
}  // namespace platform::sysconfig::grpc
