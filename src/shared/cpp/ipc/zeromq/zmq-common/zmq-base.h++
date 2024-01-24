/// -*- c++ -*-
//==============================================================================
/// @file zmq-base.h++
/// @brief Common functionality wrappers for ZeroMQ participants
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "ipc-channel.h++"
#include "logging/message/scope.h++"
#include "types/variant-value.h++"
#include "config/settingsstore.h++"
#include "version.h"            // PROJECT_NAME

#include <zmq.hpp>

#include <memory>
#include <optional>
#include <mutex>

namespace cc::zmq
{
    define_log_scope("zmq");

    // Keys to look up settings in zmq-services-*.json
    constexpr auto SETTINGS_FILE_COMMON = "zmq-channels-common";
    constexpr auto SETTINGS_FILE_PRODUCT = "zmq-channels-" PROJECT_NAME;

    constexpr auto PERSONALITY_SECTION = "personalities";
    constexpr auto DEFAULT_SECTION = "defaults";

    constexpr auto PROTOCOL_OPTION = "protocol";
    constexpr auto CONNECT_OPTION = "connect";
    constexpr auto BIND_OPTION = "listen";
    constexpr auto PORT_OPTION = "port";

    constexpr auto COMMAND_GROUP = "command";
    constexpr auto MESSAGE_GROUP = "message";

    constexpr int IO_THREADS = 1;

    class Base : public ipc::Channel
    {
        using This = Base;
        using Super = ipc::Channel;

    protected:
        Base(const std::string &class_name,
             const std::string &channel_name,
             ::zmq::socket_type socket_type);

    public:
        static std::shared_ptr<::zmq::context_t> context();
        std::shared_ptr<::zmq::socket_t> socket();

        // Get service settings
        static std::shared_ptr<SettingsStore> settings();

        // Get a specific setting.
        types::Value setting(const std::string &key,
                             const std::string &personality,
                             const types::Value &defaultValue = {}) const;

    public:
        void send(
            const types::ByteArray &bytes,
            ::zmq::send_flags flags = ::zmq::send_flags::none);

        void send(
            ::zmq::message_t &&msg,
            ::zmq::send_flags flags = ::zmq::send_flags::none);

        bool receive(
            ::zmq::message_t *msg,
            ::zmq::recv_flags flags = ::zmq::recv_flags::none);

        bool receive(
            types::ByteArray *bytes,
            ::zmq::recv_flags flags = ::zmq::recv_flags::none);

        std::optional<types::ByteArray> receive(
            ::zmq::recv_flags flags = ::zmq::recv_flags::none);

    protected:
        /// Sanitize a service address of the form `[PROTOCOL://][HOST][:PORT]`
        /// (where any or all components may be present) to the full form
        /// `PROTOCOL://HOST:PORT`.
        ///
        /// Defaults are determined as follows:
        /// * If either PROTOCOL, HOST or PORT is missing, the corresponding
        ///   option from the settings file ("ServiceName.json") is used.
        /// * Any attribute(s) that are still missing are populated from
        ///   defaultProtocol, defaultHost or defaultPort, respectively.
        ///
        /// @param[in] target
        ///     Address to sanitize, normally provided as a command-line option.
        /// @param[in] protocolOption
        ///     Key to locate the protocol name in the settings file
        /// @param[in] hostOption
        ///     Key to locate the host name in the settings file
        /// @param[in] portOption
        ///     Key to locate the port number in the settings file
        /// @param[in] defaultProtocol
        ///     Fallback if protocol name isn ot provided nor found in setitngs file
        /// @param[in] defaultHost
        ///     Fallback if host name is not provided nor found in settings file
        /// @param[in] defaultPort
        ///     Fallback if port number is not provided nor found in settings file
        /// @return
        ///     Sanitized address of the form PROTOCOL://HOST:PORT (where HOST may still be empty)

        std::string realaddress(const std::string &address,
                                const std::string &protocolOption,
                                const std::string &hostOption,
                                const std::string &portOption,
                                std::string defaultProtocol,
                                std::string defaultHost,
                                uint defaultPort = 0) const;


        virtual std::vector<std::string> settings_path() const = 0;

    private:

        // Split an address of the form `[PROTOCOL://][HOST][:PORT]` into
        // separate values
        void splitaddress(const std::string &address,
                          std::string *protocol,
                          std::string *host,
                          uint *port,
                          std::string *personality) const;

        // Join host and port into a string of the form "host:port"
        std::string joinaddress(const std::string &protocol,
                                const std::string &name,
                                uint port) const;

    private:
        static std::mutex context_mtx_;
        // static ::zmq::context_t context_;
        static std::shared_ptr<::zmq::context_t> context_;
        static std::shared_ptr<SettingsStore> settings_;

        std::shared_ptr<::zmq::socket_t> socket_;
        ::zmq::socket_type socket_type_;
    };

}  // namespace cc::zmq
