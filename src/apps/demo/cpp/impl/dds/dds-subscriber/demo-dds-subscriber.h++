// -*- c++ -*-
//==============================================================================
/// @file demo-dds-subscriber.h++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-types.hpp"  // Generated from `demo-types.idl`
#include "demo-signals.h++"
#include "dds-subscriber.h++"
#include "types/create-shared.h++"

namespace cc::demo::dds
{
    //==========================================================================
    /// @class Subscriber
    /// @brief Subscribe to and process updates from demo server
    ///
    /// Received messages are re-emitted locally via
    /// @sa cc::demo::signal_time and @sa cc::demo::signal_greeting.

    class Subscriber : public cc::dds::Subscriber,
                       public cc::types::enable_create_shared<Subscriber>
    {
        using This = Subscriber;
        using Super = cc::dds::Subscriber;

    protected:
        Subscriber(const std::string &channel_name, int domain_id);

    private:
        static void on_time_update(
            cc::signal::MappingChange change,
            const CC::Demo::TimeData &time_data);

        static void on_greeting_update(
            cc::signal::MappingChange change,
            const CC::Demo::Greeting &greeting);

    private:
        DataReaderRef<CC::Demo::TimeData> time_reader;
        DataReaderRef<CC::Demo::Greeting> greeting_reader;
    };

}  // namespace cc::demo::dds
