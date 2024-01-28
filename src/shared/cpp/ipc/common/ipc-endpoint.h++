/// -*- c++ -*-
//==============================================================================
/// @file ipc-endpoint.h++
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
    define_log_scope("ipc");

    //==========================================================================
    // @class Endpoint

    class Endpoint : public types::Streamable
    {
    protected:
        // Keys to look up settings in grpc-endpoints-*.json
        static constexpr auto PORT_OPTION = "port";
        static constexpr auto HOST_OPTION = "host";
        static constexpr auto BIND_OPTION = "interface";

    protected:
        /// @brief A generic communications endpoint, agnostic to platform
        /// @param[in] endpoint_type
        ///     Type description for this endpoint, used for debugging, etc.
        /// @param[in] channel_name
        ///     Name used to identify communications endpoint,
        ///     e.g. look up communication parameters between peers.
        Endpoint(const std::string &endpoint_type,
                 const std::string &channel_name);
        ~Endpoint();

    public:
        virtual void initialize() {}
        virtual void deinitialize() {}

        const std::string &endpoint_type() const;
        const std::string &channel_name() const;

    protected:
        void to_stream(std::ostream &stream) const override;

    private:
        std::string endpoint_type_;
        std::string channel_name_;
    };
}  // namespace cc::ipc
