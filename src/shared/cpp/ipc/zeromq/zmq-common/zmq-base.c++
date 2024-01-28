/// -*- c++ -*-
//==============================================================================
/// @file zmq-base.c++
/// @brief Common functionality wrappers for ZeroMQ services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-base.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"
#include "application/init.h++"
#include "status/exceptions.h++"
#include "buildinfo.h"  // PROJECT_NAME

namespace cc::zmq
{
    // Keys to look up settings in zmq-services-*.json
    constexpr auto SETTINGS_FILE_COMMON = "zmq-endpoints-common";
    constexpr auto SETTINGS_FILE_PRODUCT = "zmq-endpoints-" PROJECT_NAME;
    constexpr int  IO_THREADS = 2;

    Base::Base(const std::string &endpoint_type,
               const std::string &channel_name,
               ::zmq::socket_type socket_type)
        : Super(endpoint_type, channel_name),
          // socket_type_(socket_type)
          socket_(std::make_shared<::zmq::socket_t>(*Base::context(), socket_type))
    {
    }

    std::shared_ptr<::zmq::context_t> Base::context()
    {
        std::lock_guard lck(This::context_mtx_);
        if (!This::context_)
        {
            This::context_ = std::make_shared<::zmq::context_t>(IO_THREADS);
            cc::application::signal_shutdown.connect(
                TYPE_NAME_FULL(::zmq::context_t),
                std::bind(&::zmq::context_t::shutdown, This::context_));
        }
        return This::context_;
    }

    std::shared_ptr<::zmq::socket_t> Base::socket()
    {
        // if (!this->socket_)
        // {
        //     this->socket_ = std::make_shared<::zmq::socket_t>(
        //         *Base::context(),
        //         this->socket_type_);
        // }
        return this->socket_;
    }

    std::shared_ptr<SettingsStore> Base::settings()
    {
        if (!This::settings_)
        {
            This::settings_ = SettingsStore::create_shared(
                types::PathList({SETTINGS_FILE_PRODUCT,
                                 SETTINGS_FILE_COMMON}));
        }
        return This::settings_;
    }

    types::Value Base::setting(const std::string &key,
                               const types::Value &defaultValue) const
    {
        return this->settings()
            ->get(this->channel_name())
            .get(key, defaultValue);
    }

    void Base::to_stream(std::ostream &stream) const
    {
        str::format(stream,
                    "ZMQ %s (%r)",
                    this->endpoint_type(),
                    this->channel_name());
    }

    void Base::log_zmq_error(const std::string &action, const ::zmq::error_t &e)
    {
        switch (e.num())
        {
        case ETERM:
            break;

        default:
            logf_warning("%s could not %s: [%s] %s", this, action, e.num(), e.what());
            break;
        }
    }

    void Base::initialize()
    {
        Super::initialize();
    }

    void Base::deinitialize()
    {
        try
        {
            logf_debug("%s closing socket", *this);
            this->socket_->close();
        }
        catch (const ::zmq::error_t &e)
        {
            this->log_zmq_error("close socket", e);
        }
        Super::deinitialize();
    }

    void Base::send(const types::ByteArray &bytes, ::zmq::send_flags flags)
    {
        logf_trace("%s sending %d bytes", *this, bytes.size());
        this->socket()->send(::zmq::const_buffer(bytes.data(), bytes.size()), flags);
    }

    void Base::send(::zmq::message_t &&msg, ::zmq::send_flags flags)
    {
        logf_trace("%s sending %d bytes", *this, msg.size());
        this->socket()->send(msg, flags);
    }

    bool Base::receive(::zmq::message_t *msg, ::zmq::recv_flags flags)
    {
        if (this->socket()->recv(*msg, flags).has_value())
        {
            logf_trace("%s received %d bytes", *this, msg->size());
            return true;
        }
        else
        {
            logf_trace("%s received empty message", *this);
            return false;
        }
    }

    bool Base::receive(types::ByteArray *bytes, ::zmq::recv_flags flags)
    {
        ::zmq::message_t msg;
        if (this->receive(&msg, flags))
        {
            const std::string_view &sv = msg.to_string_view();
            bytes->assign(sv.begin(), sv.end());
            return true;
        }
        else
        {
            return false;
        }
    }

    std::optional<types::ByteArray> Base::receive(::zmq::recv_flags flags)
    {
        types::ByteArray bytes;
        if (this->receive(&bytes, flags))
        {
            return bytes;
        }
        else
        {
            return {};
        }
    }

    std::string Base::realaddress(const std::string &address,
                                  const std::string &protocolOption,
                                  const std::string &hostOption,
                                  const std::string &portOption,
                                  std::string defaultProtocol,
                                  std::string defaultHost,
                                  uint defaultPort) const
    {
        std::string protocol;
        std::string host;
        uint port = 0;

        this->splitaddress(address, &protocol, &host, &port);

        if (protocol.empty())
        {
            protocol = this->setting(protocolOption,
                                     defaultProtocol)
                           .as_string();
        }

        if (host.empty())
        {
            host = this->setting(hostOption,
                                 defaultHost)
                       .as_string();
        }

        if (port == 0)
        {
            port = this->setting(portOption,
                                 defaultPort)
                       .as_uint();
        }

        return this->joinaddress(protocol, host, port);
    }

    void Base::splitaddress(const std::string &address,
                            std::string *protocol,
                            std::string *host,
                            uint *port) const
    {
        static const std::regex rx(
            "(?:(\\w*)://)?"                       // protocol
            "(\\*|\\[[\\w\\.:]*\\]|[\\w\\-\\.]*)"  // host, either '[x[:x...]]' or 'n[.n]...'
            "(?::(\\d+))?"                         // port
        );

        std::smatch match;
        if (std::regex_match(address, match, rx))
        {
            *protocol = match.str(1);
            *host = match.str(2);
            *port = match.length(3) ? std::stoi(match.str(3)) : 0;
        }
        else
        {
            protocol->clear();
            host->clear();
            *port = 0;
        }
    }

    std::string Base::joinaddress(const std::string &protocol,
                                  const std::string &name,
                                  uint port) const
    {
        std::string uri = protocol + "://" + name;
        if (port != 0)
        {
            uri += ":" + std::to_string(port);
        }
        return uri;
    }

    std::mutex Base::context_mtx_;

    std::shared_ptr<::zmq::context_t> Base::context_;

    std::shared_ptr<SettingsStore> Base::settings_;

}  // namespace cc::zmq
