// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-subscriber.h++
/// @brief Subscribe to Switchboard signals and emit updates locally as signals
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switchboard-types.hpp"  // Generated from `switchboard-types.idl`
#include "switchboard.h++"
#include "dds-subscriber.h++"
#include "types/create-shared.h++"

namespace switchboard::dds
{
    //==========================================================================
    /// @class Subscriber
    /// @brief Subscribe to and process updates from switchboard server
    ///
    /// Received messages are re-emitted locally via
    /// @sa switchboard::signal_spec and @sa switchboard::signal_status.

    class Subscriber : public core::dds::Subscriber,
                       public core::types::enable_create_shared<Subscriber>
    {
        using This = Subscriber;
        using Super = core::dds::Subscriber;

    protected:
        Subscriber(const std::string &name, int domain_id);

    private:
        static void on_spec_update(core::signal::MappingAction action,
                                   const CC::Switchboard::Specification &spec);

        static void on_status_update(core::signal::MappingAction action,
                                     const CC::Switchboard::Status &status);

    private:
        DataReaderPtr<CC::Switchboard::Specification> spec_reader;
        DataReaderPtr<CC::Switchboard::Status> status_reader;
    };

    extern core::signal::MappingSignal<CC::Switchboard::Specification> signal_dds_spec;
    extern core::signal::MappingSignal<CC::Switchboard::Status> signal_dds_status;
}  // namespace switchboard::dds
