/// -*- c++ -*-
//==============================================================================
/// @file dds-channel.c++
/// @brief Mix-in base for DDS entities, to add QoS
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dds-channel.h++"
#include "logging/logging.h++"

#include <dds/domain/find.hpp>

namespace cc::dds
{
    DDS_Channel::DDS_Channel(const std::string &class_name,
                             const std::string &instance_name,
                             int domain_id)
        : Super(class_name, instance_name),
          domain_id_(domain_id)
    {
        logf_trace("DDS_Channel constructor, domain %d: %s",
                   domain_id,
                   this->to_string());
    }

    int DDS_Channel::domain_id() const
    {
        return this->domain_id_;
    }

    ::dds::domain::DomainParticipant DDS_Channel::get_participant() const
    {
        return This::get_participant(this->domain_id());
    }

    ::dds::domain::DomainParticipant DDS_Channel::get_participant(int domain_id)
    {
        if (::dds::domain::DomainParticipant participant = ::dds::domain::find(domain_id);
            participant != ::dds::core::null)
        {
            return participant;
        }
        else
        {
            return ::dds::domain::DomainParticipant(domain_id);
        }
    }
}  // namespace cc::dds
