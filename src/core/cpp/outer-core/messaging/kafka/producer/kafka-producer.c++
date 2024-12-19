/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer.h++
/// @brief Common functionality wrappers for KAFKA producers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "kafka-producer.h++"

namespace core::kafka
{
    Producer::Producer(const std::string &channel_name)
        : Super("Producer", channel_name)
    {
    }
} // namespace core::kafka
