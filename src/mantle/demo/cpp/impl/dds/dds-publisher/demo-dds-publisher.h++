// -*- c++ -*-
//==============================================================================
/// @file demo-dds-publisher.h++
/// @brief Publish demo signals over DDS
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-types.hpp"  // generated from `demo-types.idl`
#include "demo-types.h++"
#include "dds-publisher.h++"

#include "types/create-shared.h++"
#include <dds/pub/ddspub.hpp>

namespace demo::dds
{
    //==========================================================================
    // @class Publisher
    // @brief Publish locally-emitted demo signals over DDS

    class Publisher : public core::dds::Publisher,
                      public core::types::enable_create_shared<Publisher>
    {
        // Convencience alias
        using This = Publisher;
        using Super = core::dds::Publisher;

    protected:
        Publisher(const std::string &channel_name, int domain_id);

    public:
        void initialize() override;
        void deinitialize() override;

    private:
        void on_time_update(const TimeData &time_data);

        void on_greeting_update(core::signal::MappingAction change,
                                const std::string &identity,
                                const Greeting &greeting);

    private:
        DataWriterRef<CC::Demo::TimeData> time_writer;
        DataWriterRef<CC::Demo::Greeting> greeting_writer;
    };
}  // namespace demo::dds
