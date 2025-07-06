/// -*- c++ -*-
//==============================================================================
/// @file messaging-endpoint.h++
/// @brief Abstact base for a single service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/streamable.h++"
#include "types/filesystem.h++"
#include "logging/message/scope.h++"
#include "platform/symbols.h++"
#include "settings/settingsstore.h++"

#include <string>

namespace core::messaging
{
    define_log_scope("messaging");

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
        /// @param[in] messaging_flavor
        ///     Short descriptive name for messaging plavor, e.g. "gRPC", "ZMQ", "REST", ...
        /// @param[in] endpoint_type
        ///     Type description for this endpoint, used for debugging, etc.
        /// @param[in] channel_name
        ///     Name used to identify communications endpoint,
        ///     e.g. look up communication parameters between peers.
        Endpoint(const std::string &messaging_flavor,
                 const std::string &endpoint_type,
                 const std::string &channel_name,
                 const std::string &profile_name = {});
        virtual ~Endpoint();

    public:
        virtual void initialize();
        virtual void deinitialize();
        virtual bool initialized() const;

        std::string messaging_flavor() const;
        std::string endpoint_type() const;
        std::string channel_name() const;
        std::string profile_name() const;

        std::shared_ptr<SettingsStore> settings() const;
        types::Value setting(const std::string &key,
                             const types::Value &fallback = {}) const;

        std::optional<fs::path> settings_file(
            const std::string &flavor) const;

    protected:
        void to_stream(std::ostream &stream) const override;

    private:
        std::string messaging_flavor_;
        std::string endpoint_type_;
        std::string channel_name_;
        std::string profile_name_;
        std::shared_ptr<SettingsStore> settings_;
        bool initialized_;
    };
}  // namespace core::messaging
