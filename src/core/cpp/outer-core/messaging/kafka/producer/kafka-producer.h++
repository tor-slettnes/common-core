/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer.h++
/// @brief Common functionality wrappers for KAFKA producers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "kafka-endpoint.h++"

namespace core::kafka
{
    class Producer : public Endpoint
    {
        using This = Producer;
        using Super = Endpoint;

    protected:
        Producer(const std::string &channel_name);
    };
} // namespace core::kafka
