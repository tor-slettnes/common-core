// -*- c++ -*-
//==============================================================================
/// @file relay-publisher.h++
/// @brief Relay publisher abstract base
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-types.h++"

namespace relay
{
    //--------------------------------------------------------------------------
    // Abstract Publisher Interface

    class Publisher
    {
        using This = Publisher;

    public:
        virtual void initialize() {}
        virtual void deinitialize() {};
        virtual bool publish(const std::string &topic,
                             const core::types::Value &payload) = 0;

    };

}  // namespace relay
