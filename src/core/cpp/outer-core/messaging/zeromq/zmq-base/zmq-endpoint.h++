/// -*- c++ -*-
//==============================================================================
/// @file zmq-endpoint.h++
/// @brief Common functionality wrappers for ZeroMQ participants
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "messaging-endpoint.h++"
#include "logging/message/scope.h++"
#include "types/value.h++"
#include "settings/settingsstore.h++"

#if CPPZMQ
#include <zmq.hpp>
#else
#include <zmq.h>
#endif

#include <memory>
#include <optional>
#include <mutex>

namespace core::zmq
{
    define_log_scope("zmq");

#if CPPZMQ
    using Context = void;
    using Socket = void;
    using SocketType = ::zmq::socket_type;
    using SendFlags = ::zmq::send_flags;
    using RecvFlags = ::zmq::recv_flags;
    using Message = ::zmq::message_t;
    using Error = ::zmq::error_t;
#else
    using Context = void;
    using Socket = void;
    using SocketType = int;
    using SendFlags = int;
    using RecvFlags = int;
    using Message = zmq_msg_t;
    using Error = std::system_error;
#endif

    constexpr auto SCHEME_OPTION = "scheme";
    constexpr auto CONNECT_OPTION = "connect";
    constexpr auto BIND_OPTION = "listen";
    constexpr auto PORT_OPTION = "port";

    constexpr auto COMMAND_GROUP = "command";
    constexpr auto MESSAGE_GROUP = "message";

    class Endpoint : public messaging::Endpoint
    {
        using This = Endpoint;
        using Super = messaging::Endpoint;

    protected:
        Endpoint(const std::string &endpoint_type,
                 const std::string &channel_name,
                 SocketType socket_type);

        ~Endpoint();

    public:
        static Context *context();
        Socket *socket();

        void initialize() override;
        void deinitialize() override;

    protected:
        static void *check_error(void *ptr);
        static int check_error(int rc);

        void log_zmq_error(const std::string &action, const Error &e);

    public:
        void send(
            const types::ByteVector &bytes,
            SendFlags flags = 0);

        std::optional<types::ByteVector> receive(
            RecvFlags flags = 0);

        std::size_t receive(
            types::ByteVector *bytes,
            RecvFlags flags = 0);

        std::size_t receive(
            std::ostream &stream,
            RecvFlags flags = 0);

    protected:
        /// @param[in] address
        ///   Address to sanitize, normally provided as a command-line option.
        /// @param[in] schemeOption
        ///   Key to locate the scheme in the settings file
        /// @param[in] hostOption
        ///   Key to locate the host name in the settings file
        /// @param[in] portOption
        ///   Key to locate the port number in the settings file
        /// @param[in] defaultScheme
        ///   Fallback if scheme name is not provided nor found in settings file
        /// @param[in] defaultHost
        ///   Fallback if host name is not provided nor found in settings file
        /// @param[in] defaultPort
        ///   Fallback if port number is not provided nor found in settings file
        /// @return
        ///   Sanitized address of the form SCHEME://HOST:PORT (where HOST
        ///   may still be empty)
        ///
        /// Sanitize a service address of the form `[SCHEME://][HOST][:PORT]`
        /// (where any or all components may be present) to the full form
        /// `SCHEME://HOST:PORT`.
        ///
        /// If either SCHEME, HOST or PORT is missing, defaults are determined as
        /// follows:
        ///
        /// * If the product-specific settings file
        ///   `*-endpoints-PRODUCT_NAME.json` contains a map entry for this ZMQ
        ///   channel name, the value is extracted from this map using the
        ///   corresponding argument `schemeOption`, `hostOption` or
        ///   `portOption` as key.
        ///
        /// * If still missing, the same lookup is performed in the file
        ///   `*-endpoints-common.json`.
        ///
        /// * Any attribute(s) that are still missing are populated from
        ///   `defaultScheme`, `defaultHost` or `defaultPort`, respectively.

        std::string realaddress(const std::string &address,
                                const std::string &schemeOption,
                                const std::string &hostOption,
                                const std::string &portOption,
                                std::string defaultScheme,
                                std::string defaultHost,
                                uint defaultPort = 0) const;

    private:
        // Split an address of the form `[SCHEME://][HOST][:PORT]` into
        // separate values
        void splitaddress(const std::string &address,
                          std::string *scheme,
                          std::string *host,
                          uint *port) const;

        // Join host and port into a string of the form "host:port"
        std::string joinaddress(const std::string &scheme,
                                const std::string &name,
                                uint port) const;

    private:
        static std::mutex context_mtx_;
        // static Context context_;
        static Context *context_;

        Socket *socket_;
        SocketType socket_type_;
    };

}  // namespace core::zmq
