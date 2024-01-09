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

namespace cc::demo::dds
{
    //==========================================================================
    // @class Publisher
    // @brief Publish locally-emitted demo signals over DDS

    class Publisher : public cc::dds::DDS_Publisher,
                      public cc::types::enable_create_shared<Publisher>
    {
        // Convencience alias
        using This = Publisher;
        using Super = cc::dds::DDS_Publisher;

    protected:
        Publisher(const std::string &name, int domain_id);

    public:
        void initialize() override;
        void deinitialize() override;

    private:
        void on_time_update(const TimeData &time_data);

        void on_greeting_update(cc::signal::MappingChange change,
                                const std::string &identity,
                                const Greeting &greeting);

    private:
        DataWriterRef<CC::Demo::TimeData> time_writer;
        DataWriterRef<CC::Demo::Greeting> greeting_writer;
    };
}  // namespace cc::demo::dds
