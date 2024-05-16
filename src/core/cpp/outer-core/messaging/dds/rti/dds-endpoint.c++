/// -*- c++ -*-
//==============================================================================
/// @file dds-endpoint.c++
/// @brief Mix-in base for DDS entities, to add QoS
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dds-endpoint.h++"
#include "logging/logging.h++"

#include <dds/domain/find.hpp>

namespace core::dds
{
    Endpoint::Endpoint(const std::string &endpoint_type,
                       const std::string &channel_name,
                       int domain_id)
        : Super("DDS", endpoint_type, channel_name),
          domain_id_(domain_id)
    {
    }

    int Endpoint::domain_id() const
    {
        return this->domain_id_;
    }

    ::dds::domain::DomainParticipant Endpoint::get_participant() const
    {
        return This::get_participant(this->domain_id());
    }

    ::dds::domain::DomainParticipant Endpoint::get_participant(int domain_id)
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

}  // namespace core::dds
