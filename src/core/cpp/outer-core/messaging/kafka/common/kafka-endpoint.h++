/// -*- c++ -*-
//==============================================================================
/// @file kafka-endpoint.h++
/// @brief Common functionality wrappers for KAFKA endpoints
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "messaging-endpoint.h++"
#include "types/value.h++"


namespace core::kafka
{
    class Endpoint : public messaging::Endpoint
    {
        using This = Endpoint;
        using Super = messaging::Endpoint;

    protected:
        Endpoint(const std::string &endpoint_type,
                 const std::string &channel_name);

        ~Endpoint();
    };
}
