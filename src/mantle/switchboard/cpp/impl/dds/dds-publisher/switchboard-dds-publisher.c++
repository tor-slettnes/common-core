// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-publisher.c++
/// @brief Publish switchboard signals over DDS
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "switchboard-dds-publisher.h++"
#include "translate-idl-switchboard.h++"
#include "translate-idl-inline.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"

namespace switchboard::dds
{
    Publisher::Publisher(const std::string &name, int domain_id)
        : Super(name, domain_id),
          spec_writer(this->create_writer<CC::Switchboard::Specification>(
              CC::Switchboard::SPEC_TOPIC,  // topic_name
              true,                         // reliable
              true)),                       // sync_latest
          status_writer(this->create_writer<CC::Switchboard::Status>(
              CC::Switchboard::STATUS_TOPIC,  // topic_name
              true,                           // reliable
              true))                          // sync_latest
    {
        logf_debug("Switchboard publisher");
    }

    void Publisher::initialize()
    {
        using namespace std::placeholders;

        Super::initialize();

        signal_spec.connect(
            this->to_string(),
            std::bind(&Publisher::on_spec_update, this, _1, _2, _3));

        signal_status.connect(
            this->to_string(),
            std::bind(&Publisher::on_status_update, this, _1, _2, _3));
    }

    void Publisher::deinitialize()
    {
        signal_status.disconnect(this->to_string());
        signal_spec.disconnect(this->to_string());
        Super::deinitialize();
    }

    void Publisher::on_spec_update(core::signal::MappingAction action,
                                   const SwitchName &name,
                                   const Specification &spec)
    {
        logf_trace("Sending switch %r %s spec update: %s", name, action, spec);
        auto encoded_spec = idl::encoded<CC::Switchboard::Specification>(name, spec);
        this->publish_change(this->spec_writer, action, encoded_spec);
    }

    void Publisher::on_status_update(core::signal::MappingAction action,
                                     const SwitchName &name,
                                     const Status &status)
    {
        logf_trace("Sending switch %r %s status update: %s", name, action, status);
        auto encoded_status = idl::encoded<CC::Switchboard::Status>(name, status);
        this->publish_change(this->status_writer, action, encoded_status);
    }

}  // namespace switchboard::dds
