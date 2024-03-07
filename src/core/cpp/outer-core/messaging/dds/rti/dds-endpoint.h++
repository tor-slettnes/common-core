/// -*- c++ -*-
//==============================================================================
/// @file dds-endpoint.h++
/// @brief Mix-in base for DDS entities, to add QoS
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "messaging-endpoint.h++"
#include <dds/core/policy/CorePolicy.hpp>
#include <dds/domain/DomainParticipant.hpp>

#include <thread>

namespace core::dds
{
    //==========================================================================
    /// @brief Mix-in base for publishers & subscribers

    class Endpoint : public messaging::Endpoint
    {
        using This = Endpoint;
        using Super = messaging::Endpoint;

    protected:
        // Inherit constructor
        Endpoint(const std::string &endpoint_type,
                 const std::string &channel_name,
                 int domain_id);

        template <class QoS>
        static QoS pubsub_policy(bool reliable = true,
                                 bool sync_latest = false)
        {
            QoS qos;
            qos << (reliable
                        ? ::dds::core::policy::Reliability::Reliable()
                        : ::dds::core::policy::Reliability::BestEffort());

            qos << ::dds::core::policy::History::KeepLast(1);

            qos << (sync_latest
                        ? ::dds::core::policy::Durability::TransientLocal()
                        : ::dds::core::policy::Durability::Volatile());

            return qos;
        }

    public:
        int domain_id() const;

    protected:
        ::dds::domain::DomainParticipant get_participant() const;
        static ::dds::domain::DomainParticipant get_participant(int domain_id);

    public:
        int domain_id_;
    };
}  // namespace core::dds
