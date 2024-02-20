/// -*- c++ -*-
//==============================================================================
/// @file grpc-base.h++
/// @brief Common functionality wrappers for gRPC client and server modules
/// @author Tor Slettnes <tor@slett.net>
///
/// Class templates for gRPC services (client and server), including:
///  * Settings store in YourServiceName.json
///  * Status/error code wrappers
//==============================================================================

#pragma once
#include "grpc-base.h++"
#include "grpc-status.h++"
#include "messaging-endpoint.h++"
#include "config/settingsstore.h++"
#include <grpc++/grpc++.h>

#include <google/protobuf/empty.pb.h>

namespace shared::grpc
{
    define_log_scope("grpc");

    //==========================================================================
    /// @class Base
    /// @brief
    ///     General purpose mix-in base class for ClientBase and
    ///     ServerBase.

    class Base : public messaging::Endpoint
    {
        using This = Base;
        using Super = messaging::Endpoint;

    protected:
        // Keys to look up settings in grpc-endpoints-*.json
        static constexpr auto MAX_REQUEST_SIZE = "max request size";
        static constexpr auto MAX_REPLY_SIZE = "max reply size";
        static constexpr auto PORT_OPTION = "port";
        static constexpr auto HOST_OPTION = "host";
        static constexpr auto BIND_OPTION = "interface";

    protected:
        Base(const std::string &endpoint_type,
             const std::string &full_service_name);

    public:
        /// @fn servicename
        /// @brief
        ///     Return the name of this service
        /// @param[in] full
        ///     Obtain the full name (`"package.Name"`), otherwise just the stem (`"Name"`).
        /// @return
        ///     Service name
        std::string servicename(bool full = false) const;

        fs::path settings_file(const std::string &product) const override;
    protected:
        /// Sanitize a target address of the form [HOST][:PORT] (where either or both
        /// may be absent) by ensuring it contains both.
        ///
        /// If either [HOST] or [:PORT] is missing, the corresponding option from the
        /// settings file ("ServiceName.json") is used. If PORT is still missing but
        /// "servicename" is defined in the settings file, the corresponding value
        /// used to find the service port via netdb (i.e., /etc/services).  Finally,
        /// if all else fails, the missing attribute is populated from defaultHost or
        /// defaultPort, respectively.
        ///
        /// @param[in] address
        ///     Address to sanitize, normally provided as a command-line option.
        /// @param[in] channel_name
        ///     Key to locate endpoint settings within the settings file
        /// @param[in] hostOption
        ///     Key to locate the host name in the channel settings
        /// @param[in] portOption
        ///     Key to locate the port number in the channel settings
        /// @param[in] defaultHost
        ///     Fallback if host name is not provided nor found
        /// @param[in] defaultPort
        ///     Fallback if port number is not provided nor found
        /// @return
        ///     Sanitized address of the form HOST:PORT (where HOST may still be empty)

        std::string realaddress(const std::string &address,
                                const std::string &hostOption,
                                const std::string &portOption,
                                std::string defaultHost,
                                uint defaultPort) const;

    public:
        /// Obtain max. message size configuration
        uint max_request_size() const;

        uint max_reply_size() const;

    private:
        // Split an address of the form [PERSONALITY@][HOST][:PORT] into
        // separate values
        void splitaddress(const std::string &address,
                          std::string *host,
                          uint *port) const;

        // Join host and port into a string of the form "host:port"
        std::string joinaddress(const std::string &host, uint port) const;

    private:
        static std::shared_ptr<SettingsStore> settings_;
        const std::string full_service_name_;
    };

}  // namespace shared::grpc
