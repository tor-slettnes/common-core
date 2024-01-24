/// -*- c++ -*-
//==============================================================================
/// @file dds-publisher.c++
/// @brief A publishing entity/producer in a DDS environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dds-publisher.h++"
#include "logging/logging.h++"

namespace cc::dds
{
    Publisher::Publisher(
        const std::string &class_name,
        const std::string &channel_name,
        int domain_id)
        : Super(class_name, channel_name, domain_id),
          ::dds::pub::Publisher(this->get_participant())
    {
    }
}  // namespace cc::dds
