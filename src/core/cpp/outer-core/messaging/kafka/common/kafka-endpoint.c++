/// -*- c++ -*-
//==============================================================================
/// @file kafka-endpoint.c++
/// @brief Common functionality wrappers for KAFKA endpoints
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "kafka-endpoint.h++"

namespace core::kafka
{
    Endpoint::Endpoint(const std::string &endpoint_type,
                       const std::string &channel_name)
        : Super("Kafka", endpoint_type, channel_name)
    {
    }

    Endpoint::~Endpoint()
    {
    }
} // namespace core::kafka
