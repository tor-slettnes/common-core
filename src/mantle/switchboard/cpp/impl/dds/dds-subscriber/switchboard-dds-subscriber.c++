// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-subscriber.c++
/// @brief Subscribe to Switchboard signals and emit updates locally as signals
/// @author Tor Slettnes
//==============================================================================

#include "switchboard-dds-subscriber.h++"
#include "translate-idl-switchboard.h++"
#include "translate-idl-inline.h++"

#include <functional>

namespace switchboard::dds
{
    using namespace std::placeholders;

    Subscriber::Subscriber(const std::string &name, int domain_id)
        : Super(name, domain_id),
          spec_reader(this->create_reader<CC::Switchboard::Specification>(
              CC::Switchboard::SPEC_TOPIC,  // topic_name
              Subscriber::on_spec_update,   // handler
              true,                         // reliable
              true)),                       // sync_latest
          status_reader(this->create_reader<CC::Switchboard::Status>(
              CC::Switchboard::STATUS_TOPIC,  // topic_name
              Subscriber::on_status_update,   // handler
              true,                           // reliable
              true))                          // sync_latest
    {
    }

    void Subscriber::on_spec_update(core::signal::MappingAction action,
                                    const CC::Switchboard::Specification &spec)
    {
        logf_trace("Received spec %s: %s", action, spec);
        signal_dds_spec.emit(action, spec.switch_name(), spec);
    }

    void Subscriber::on_status_update(core::signal::MappingAction action,
                                      const CC::Switchboard::Status &status)
    {
        logf_trace("Received status %s: %s", action, status);
        signal_dds_status.emit(action, status.switch_name(), status);
    }

    core::signal::MappingSignal<CC::Switchboard::Specification> signal_dds_spec("signal_dds_spec");
    core::signal::MappingSignal<CC::Switchboard::Status> signal_dds_status("signal_dds_status");
}  // namespace switchboard::dds
