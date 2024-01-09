/// -*- c++ -*-
//==============================================================================
/// @file ipc-channel.h++
/// @brief Abstact base for a single service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/streamable.h++"
#include "logging/message/scope.h++"

#include <string>

namespace cc::ipc
{
    define_log_scope("service");

    //==========================================================================
    // @class Channel

    class Channel : public types::Streamable
    {
    protected:
        Channel(const std::string &class_name, const std::string &instance_name = {});
        virtual ~Channel();

    public:
        const std::string &class_name() const;
        const std::string &instance_name() const;

        virtual void initialize() {}
        virtual void deinitialize() {}

    protected:
        void to_stream(std::ostream &stream) const override;

    private:
        std::string class_name_;
        std::string instance_name_;
    };
}  // namespace cc::ipc
