/// -*- c++ -*-
//==============================================================================
/// @file zmq-endpoint.c++
/// @brief Common functionality wrappers for ZeroMQ services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-endpoint.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"
#include "platform/init.h++"
#include "status/exceptions.h++"

namespace core::zmq
{
    Endpoint::Endpoint(const std::string &address,
                       const std::string &endpoint_type,
                       const std::string &channel_name,
                       SocketType socket_type,
                       Role role)
        : Super("ZMQ", endpoint_type, channel_name),
          socket_(this->check_error(::zmq_socket(Endpoint::context(), socket_type))),
          role_(role),
          address_(address)
    {
    }

    Endpoint::~Endpoint()
    {
        if (this->socket_)
        {
            logf_notice("Closing %s socket", this->to_string());

            ::zmq_close(this->socket_);
            this->socket_ = nullptr;
        }
    }

    Context *Endpoint::context()
    {
        std::lock_guard lck(This::context_mtx_);
        if (!This::context_)
        {
            This::context_ = This::check_error(::zmq_ctx_new());
            core::platform::signal_shutdown.connect(
                TYPE_NAME_FULL(Context),
                std::bind(::zmq_ctx_destroy, This::context_));
        }
        return This::context_;
    }

    Socket *Endpoint::socket() const
    {
        return this->socket_;
    }

    Endpoint::Role Endpoint::role() const
    {
        return this->role_;
    }

    std::string Endpoint::address() const
    {
        return this->address_;
    }

    std::string Endpoint::bind_address(const std::string &provided) const
    {
        return this->realaddress(provided,
                                 SCHEME_OPTION,
                                 BIND_OPTION,
                                 PORT_OPTION,
                                 "tcp",
                                 "*");
    }

    void Endpoint::bind(const std::string &address)
    {
        std::string bind_address = this->bind_address(address);
        logf_debug("%s binding to %s", *this, bind_address);
        this->check_error(
            ::zmq_bind(this->socket(), bind_address.c_str()));
        this->address_ = bind_address;
    }

    void Endpoint::unbind()
    {
        // Obtain real endpoint
        if (auto endpoint = this->get_last_endpoint())
        {
            logf_debug("%s unbinding from %s", *this, endpoint.value());
            this->try_or_log(
                ::zmq_unbind(this->socket(), endpoint->c_str()),
                "could not unbind from " + endpoint.value());
        }
    }

    std::string Endpoint::host_address(const std::string &provided) const
    {
        return this->realaddress(provided,
                                 SCHEME_OPTION,
                                 CONNECT_OPTION,
                                 PORT_OPTION,
                                 "tcp",
                                 "localhost");
    }

    void Endpoint::connect(const std::string &address)
    {
        std::string host_address = this->host_address(address);
        logf_debug("%s connecting to %s", *this, host_address);
        this->check_error(
            ::zmq_connect(this->socket(), host_address.c_str()));
        this->address_ = host_address;
    }

    void Endpoint::disconnect()
    {
        if (auto endpoint = this->get_last_endpoint())
        {
            logf_debug("%s disconnecting from %s", *this, endpoint.value());
            this->try_or_log(
                ::zmq_disconnect(this->socket(), endpoint->c_str()),
                "could not disconnect from " + endpoint.value());
        }
    }

    void Endpoint::initialize()
    {
        Super::initialize();
        switch (this->role())
        {
        case Role::HOST:
            this->bind(this->address());
            break;

        case Role::SATELLITE:
            this->connect(this->address());
            break;

        default:
            break;
        }
    }

    void Endpoint::deinitialize()
    {
        switch (this->role())
        {
        case Role::HOST:
            this->unbind();
            break;

        case Role::SATELLITE:
            this->disconnect();
            break;

        default:
            break;
        }

        if (this->socket_)
        {
            ::zmq_close(this->socket_);
            this->socket_ = nullptr;
        }
        Super::deinitialize();
    }

    void *Endpoint::check_error(void *ptr)
    {
        if (ptr == nullptr)
        {
            throw std::system_error(errno, std::generic_category());
        }
        return ptr;
    }

    int Endpoint::check_error(int rc)
    {
        if (rc < 0)
        {
            throw std::system_error(errno, std::generic_category());
        }
        return rc;
    }

    std::optional<std::string> Endpoint::get_last_endpoint() const
    {
        size_t buf_size = 256;
        char buf[buf_size];
        int rc = ::zmq_getsockopt(this->socket(), ZMQ_LAST_ENDPOINT, buf, &buf_size);
        if (rc == 0)
        {
            return std::string(buf, buf_size);
        }
        else
        {
            return {};
        }
    }

    void Endpoint::try_or_log(int rc, const std::string &preamble) const
    {
        try
        {
            this->check_error(rc);
        }
        catch (const Error &e)
        {
            this->log_zmq_error(preamble, e);
        }
    }

    void Endpoint::log_zmq_error(const std::string &action, const Error &e) const
    {
        switch (e.code().value())
        {
        case ETERM:
            break;

        default:
            logf_warning("%s %s: [%s] %s", *this, action, e.code().value(), e.what());
            break;
        }
    }

    void Endpoint::send(const types::ByteVector &bytes, SendFlags flags) const
    {
        using SendFunction = int (*)(void *socket, const void *buf, size_t len, int flags);
        SendFunction send = (flags & ZMQ_DONTWAIT) ? ::zmq_send : ::zmq_send_const;

        logf_trace("%s sending %d bytes", *this, bytes.size());
        this->check_error(::zmq_send(
            this->socket(),  // socket
            bytes.data(),    // buf
            bytes.size(),    // len
            flags));         // flags
    }

    std::optional<types::ByteVector> Endpoint::receive(RecvFlags flags) const
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

    std::size_t Endpoint::receive(types::ByteVector *bytes, RecvFlags flags) const
    {
        zmq_msg_t msg;
        std::vector<std::string> counts;
        std::size_t total;

        do
        {
            this->check_error(::zmq_msg_init(&msg));
            this->check_error(::zmq_msg_recv(&msg, this->socket(), flags));

            const char *data = static_cast<const char *>(::zmq_msg_data(&msg));
            size_t size = ::zmq_msg_size(&msg);

            bytes->insert(bytes->end(), data, data + size);

            counts.push_back(std::to_string(size));
            total += size;

            this->check_error(::zmq_msg_close(&msg));
        }
        while (::zmq_msg_more(&msg));

        logf_trace("%s received %s = %d bytes",
                   *this,
                   str::join(counts, "+"),
                   total);
        return total;
    }

    std::size_t Endpoint::receive(std::ostream &stream, RecvFlags flags) const
    {
        zmq_msg_t msg;
        std::vector<std::string> counts;
        std::size_t total;

        do
        {
            this->check_error(::zmq_msg_init(&msg));
            this->check_error(::zmq_msg_recv(&msg, this->socket(), flags));

            const char *data = static_cast<const char *>(::zmq_msg_data(&msg));
            size_t size = ::zmq_msg_size(&msg);

            stream.write(data, size);

            counts.push_back(std::to_string(size));
            total += size;

            this->check_error(::zmq_msg_close(&msg));
        }
        while (::zmq_msg_more(&msg));

        logf_trace("%s received %s = %d bytes",
                   *this,
                   str::join(counts, "+"),
                   total);
        return total;
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

    Context *Endpoint::context_ = nullptr;

}  // namespace core::zmq
