/// -*- c++ -*-
//==============================================================================
/// @file ipc-channel.h++
/// @brief Abstact base for a single service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/streamable.h++"
#include "logging/message/scope.h++"
#include "platform/symbols.h++"

#include <string>

namespace cc::ipc
{
    define_log_scope("service");

    //==========================================================================
    // @class Channel

    class Channel : public types::Streamable
    {
    protected:
        /// @brief A generic communications channel, agnostic to platform
        /// @param[in] class_name
        ///     Final implementation class, used for handles, debugging, etc.
        /// @param[in] channel_name
        ///     Name used to identify communications channel,
        ///     e.g. look up communication parameters between peers.
        Channel(const std::string &class_name,
                const std::string &channel_name);
        ~Channel();

    public:
        const std::string &class_name() const;
        const std::string &channel_name() const;

        virtual void initialize() {}
        virtual void deinitialize() {}

    protected:
        void to_stream(std::ostream &stream) const override;

    private:
        std::string channel_name_;
        std::string class_name_;
    };
}  // namespace cc::ipc
