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
    DDS_Publisher::DDS_Publisher(
        const std::string &type,
        const std::string &name,
        int domain_id)
        : Super(type, name, domain_id),
          ::dds::pub::Publisher(this->get_participant())
    {
    }
}  // namespace cc::dds
