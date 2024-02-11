/// -*- c++ -*-
//==============================================================================
/// @file zmq-endpoint.c++
/// @brief Common functionality wrappers for ZeroMQ services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-endpoint.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"
#include "application/init.h++"
#include "status/exceptions.h++"
#include "buildinfo.h"  // PROJECT_NAME

namespace cc::zmq
{
    // Keys to look up settings in zmq-services-*.json
    constexpr int IO_THREADS = 2;

    Endpoint::Endpoint(const std::string &endpoint_type,
                       const std::string &channel_name,
                       ::zmq::socket_type socket_type)
        : Super("ZMQ", endpoint_type, channel_name),
          // socket_type_(socket_type)
          socket_(std::make_shared<::zmq::socket_t>(*Endpoint::context(), socket_type))
    {
    }

    std::shared_ptr<::zmq::context_t> Endpoint::context()
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

    std::shared_ptr<::zmq::socket_t> Endpoint::socket()
    {
        // if (!this->socket_)
        // {
        //     this->socket_ = std::make_shared<::zmq::socket_t>(
        //         *Endpoint::context(),
        //         this->socket_type_);
        // }
        return this->socket_;
    }

    void Endpoint::log_zmq_error(const std::string &action, const ::zmq::error_t &e)
    {
        switch (e.num())
        {
        case ETERM:
            break;

        default:
            logf_warning("%s %s: [%s] %s", *this, action, e.num(), e.what());
            break;
        }
    }

    void Endpoint::initialize()
    {
        Super::initialize();
    }

    void Endpoint::deinitialize()
    {
        try
        {
            logf_debug("%s closing socket", *this);
            this->socket_->close();
        }
        catch (const ::zmq::error_t &e)
        {
            this->log_zmq_error("could not close socket", e);
        }
        Super::deinitialize();
    }

    void Endpoint::send(const types::ByteVector &bytes, ::zmq::send_flags flags)
    {
        logf_trace("%s sending %d bytes", *this, bytes.size());
        this->socket()->send(::zmq::const_buffer(bytes.data(), bytes.size()), flags);
    }

    bool Endpoint::receive(types::ByteVector *bytes, ::zmq::recv_flags flags)
    {
        bool received = false;
        ::zmq::message_t msg;
        std::vector<std::string> counts;
        do
        {
            if (this->receive_chunk(&msg, flags))
            {
                types::Byte *data = static_cast<types::Byte *>(msg.data());
                bytes->insert(bytes->end(), data, data + msg.size());
                counts.push_back(std::to_string(msg.size()));
                received = true;
            }
        }
        while (msg.more());
        logf_trace("%s received %s = %d bytes",
                   *this,
                   str::join(counts, "+"),
                   bytes->size());
        return received;
    }

    std::optional<types::ByteVector> Endpoint::receive(::zmq::recv_flags flags)
    {
        types::ByteVector bytes;
        if (this->receive(&bytes, flags))
        {
            return bytes;
        }
        else
        {
            return {};
        }
    }

    bool Endpoint::receive_chunk(::zmq::message_t *msg, ::zmq::recv_flags flags)
    {
        if (this->socket()->recv(*msg, flags).has_value())
        {
            logf_trace("%s received %d bytes: %s",
                       *this,
                       msg->size(),
                       msg->to_string_view());
            return true;
        }
        else
        {
            logf_trace("%s received empty message", *this);
            return false;
        }
    }


    std::string Endpoint::realaddress(const std::string &address,
                                      const std::string &schemeOption,
                                      const std::string &hostOption,
                                      const std::string &portOption,
                                      std::string defaultScheme,
                                      std::string defaultHost,
                                      uint defaultPort) const
    {
        std::string scheme;
        std::string host;
        uint port = 0;

        this->splitaddress(address, &scheme, &host, &port);

        if (scheme.empty())
        {
            scheme = this->setting(schemeOption,
                                   defaultScheme)
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

        return this->joinaddress(scheme, host, port);
    }

    void Endpoint::splitaddress(const std::string &address,
                                std::string *scheme,
                                std::string *host,
                                uint *port) const
    {
        static const std::regex rx(
            "(?:(\\w*)://)?"                       // scheme
            "(\\*|\\[[\\w\\.:]*\\]|[\\w\\-\\.]*)"  // host, either '[x[:x...]]' or 'n[.n]...'
            "(?::(\\d+))?"                         // port
        );

        std::smatch match;
        if (std::regex_match(address, match, rx))
        {
            *scheme = match.str(1);
            *host = match.str(2);
            *port = match.length(3) ? std::stoi(match.str(3)) : 0;
        }
        else
        {
            scheme->clear();
            host->clear();
            *port = 0;
        }
    }

    std::string Endpoint::joinaddress(const std::string &scheme,
                                      const std::string &name,
                                      uint port) const
    {
        std::string uri = scheme + "://" + name;
        if (port != 0)
        {
            uri += ":" + std::to_string(port);
        }
        return uri;
    }

    std::mutex Endpoint::context_mtx_;

    std::shared_ptr<::zmq::context_t> Endpoint::context_;

    std::shared_ptr<SettingsStore> Endpoint::settings_;

}  // namespace cc::zmq
