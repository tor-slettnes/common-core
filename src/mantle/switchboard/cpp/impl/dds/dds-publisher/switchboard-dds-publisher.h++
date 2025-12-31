// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-publisher.h++
/// @brief Publish switchboard signals over DDS
/// @author Tor Slettnes
//==============================================================================

#pragma once

#include "switchboard-types.hpp"  // generated from `switchboard-types.idl`
#include "switchboard.h++"
#include "dds-publisher.h++"

#include "types/create-shared.h++"
#include <dds/pub/ddspub.hpp>

//#include "switchboard-interface.h++" // generated from `switchboard-service.idl`

namespace switchboard::dds
{
    //==========================================================================
    // @class Publisher
    // @brief Publish updates to Switchboard clients

    class Publisher : public core::dds::Publisher,
                      public core::types::enable_create_shared<Publisher>
    {
        // Convencience alias
        using This = Publisher;
        using Super = core::dds::Publisher;

    protected:
        Publisher(const std::string &name, int domain_id);

    public:
        void initialize() override;
        void deinitialize() override;

    private:
        void on_spec_update(core::signal::MappingAction action,
                            const SwitchName &name,
                            const Specification &spec);

        void on_status_update(core::signal::MappingAction action,
                              const SwitchName &name,
                              const Status &status);

    private:
        DataWriterPtr<CC::Switchboard::Specification> spec_writer;
        DataWriterPtr<CC::Switchboard::Status> status_writer;
    };
}  // namespace switchboard::dds
