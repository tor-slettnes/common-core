/// -*- c++ -*-
//==============================================================================
/// @file ipc-endpoint.c++
/// @brief Abstact base for a single service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "ipc-endpoint.h++"

#include <iomanip>
#include <sstream>

namespace cc::ipc
{
    //==========================================================================
    // @class Endpoint

    Endpoint::Endpoint(const std::string &endpoint_type,
                       const std::string &channel_name)
        : endpoint_type_(endpoint_type),
          channel_name_(channel_name)
    {
    }

    Endpoint::~Endpoint()
    {
        this->deinitialize();
    }

    const std::string &Endpoint::endpoint_type() const
    {
        return this->endpoint_type_;
    }

    const std::string &Endpoint::channel_name() const
    {
        return this->channel_name_;
    }

    void Endpoint::to_stream(std::ostream &stream) const
    {
        stream << this->endpoint_type()
               << "("
               << std::quoted(this->channel_name())
               << ")";
    }

}  // namespace cc::ipc
