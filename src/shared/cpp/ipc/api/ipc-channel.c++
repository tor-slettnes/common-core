/// -*- c++ -*-
//==============================================================================
/// @file ipc-channel.c++
/// @brief Abstact base for a single service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "ipc-channel.h++"

#include <iomanip>

namespace cc::ipc
{
    //==========================================================================
    // @class Channel

    Channel::Channel(const std::string &class_name,
                     const std::string &instance_name)
        : class_name_(class_name),
          instance_name_(instance_name)
    {
    }

    Channel::~Channel()
    {
        this->deinitialize();
    }

    const std::string &Channel::class_name() const
    {
        return this->class_name_;
    }

    const std::string &Channel::instance_name() const
    {
        return this->instance_name_;
    }

    void Channel::to_stream(std::ostream &stream) const
    {
        stream << this->class_name() << "(";
        if (!this->instance_name().empty())
        {
            stream << std::quoted(this->instance_name());
        }
        stream << ")";
    }

}  // namespace cc::ipc
