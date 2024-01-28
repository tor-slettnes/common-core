/// -*- c++ -*-
//==============================================================================
/// @file dds-publisher.c++
/// @brief A publishing entity/producer in a DDS environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dds-publisher.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"

namespace cc::dds
{
    Publisher::Publisher(const std::string &channel_name, int domain_id)
        : Super(TYPE_NAME_BASE(This), channel_name, domain_id),
          ::dds::pub::Publisher(this->get_participant())
    {
    }
}  // namespace cc::dds
