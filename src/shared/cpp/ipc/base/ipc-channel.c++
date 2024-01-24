/// -*- c++ -*-
//==============================================================================
/// @file ipc-channel.c++
/// @brief Abstact base for a single service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "ipc-channel.h++"
#include "string/format.h++"

#include <iomanip>

namespace cc::ipc
{
    //==========================================================================
    // @class Channel

    Channel::Channel(const std::string &class_name,
                     const std::string &channel_name)
        : class_name_(class_name),
          channel_name_(channel_name)
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

    const std::string &Channel::channel_name() const
    {
        return this->channel_name_;
    }

    void Channel::to_stream(std::ostream &stream) const
    {
        str::format(stream, "%s(%r)", this->class_name(), this->channel_name());
    }

}  // namespace cc::ipc
