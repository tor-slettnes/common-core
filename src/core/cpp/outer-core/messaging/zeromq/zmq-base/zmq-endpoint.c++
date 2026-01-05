/// -*- c++ -*-
//==============================================================================
/// @file zmq-endpoint.c++
/// @brief Common functionality wrappers for ZeroMQ services
/// @author Tor Slettnes
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
          socket_(nullptr),
          socket_type_(socket_type),
          role_(role),
          address_(address),
          deinit_signal_handle_(this->to_string() + ".deinitialize")
    {
    }

    Endpoint::~Endpoint()
    {
        this->close_socket();
    }

    Context *Endpoint::context()
    {
        if (!This::context_)
        {
            This::context_ = This::check_error(::zmq_ctx_new());
            This::check_error(::zmq_ctx_set(This::context_, ZMQ_IPV6, 1));

            core::platform::signal_shutdown.connect(
                "terminate_zmq_context",
                This::terminate_context);
        }
        return This::context_;
    }

    void Endpoint::terminate_context()
    {
        core::platform::signal_shutdown.disconnect(
            "terminate_zmq_context");

        if (This::context_)
        {
            logf_debug("Terminating ZMQ context");
            ::zmq_ctx_term(This::context_);
            logf_debug("Terminated ZMQ context");
        }
    }

    Endpoint::Role Endpoint::role() const
    {
        return this->role_;
    }

    std::string Endpoint::address() const
    {
        return this->address_;
    }

    Socket *Endpoint::socket() const
    {
        if (!this->socket_)
        {
            throwf(core::exception::FailedPrecondition,
                   "%s socket is not open.",
                   *this);
        }

        return this->socket_;
    }

    void Endpoint::open_socket()
    {
        std::scoped_lock lck(this->socket_mtx_);
        if (!this->socket_)
        {
            logf_trace("Opening %s socket", *this);
            this->socket_ = this->check_error(::zmq_socket(
                Endpoint::context(),
                this->socket_type_));
            logf_trace("Opened %s socket", *this);
        }
    }

    void Endpoint::close_socket()
    {
        std::scoped_lock lck(this->socket_mtx_);
        if (this->socket_)
        {
            logf_trace("Closing %s socket", *this);
            ::zmq_close(this->socket_);
            this->socket_ = nullptr;
            logf_trace("Closed %s socket", *this);
        }
    }

    std::string Endpoint::bind_address(const std::optional<std::string> &provided) const
    {
        return this->realaddress(provided.value_or(this->address_),
                                 SCHEME_OPTION,
                                 BIND_OPTION,
                                 PORT_OPTION,
                                 "tcp",
                                 "*");
    }

    void Endpoint::bind(const std::optional<std::string> &address)
    {
        std::scoped_lock lck(this->socket_mtx_);
        std::string bind_address = this->bind_address(address);
        logf_debug("%s binding to %s", *this, bind_address);

        this->check_error(
            ::zmq_bind(this->socket(), bind_address.c_str()));
        this->address_ = bind_address;
    }

    void Endpoint::unbind()
    {
        std::scoped_lock lck(this->socket_mtx_);
        if (this->socket_)
        {
            // Obtain real endpoint
            std::string endpoint = this->get_last_address();
            logf_debug("%s unbinding from %s", *this, endpoint);
            this->try_or_log(
                ::zmq_unbind(this->socket_, endpoint.c_str()),
                "could not unbind from " + endpoint);
        }
    }

    std::string Endpoint::host_address(const std::optional<std::string> &provided) const
    {
        return this->realaddress(provided.value_or(this->address_),
                                 SCHEME_OPTION,
                                 HOST_OPTION,
                                 PORT_OPTION,
                                 "tcp",
                                 "localhost");
    }

    void Endpoint::connect(const std::optional<std::string> &address,
                           const std::optional<core::dt::Duration> &timeout)
    {
        std::string host_address = this->host_address(address);
        logf_debug("%s connecting to %s", *this, host_address);
        if (timeout)
        {
            this->setsockopt(ZMQ_CONNECT_TIMEOUT,
                             core::dt::to_milliseconds(timeout.value()));
        }

        std::scoped_lock lck(this->socket_mtx_);
        this->check_error(
            ::zmq_connect(this->socket(), host_address.c_str()));
        this->address_ = host_address;
    }

    void Endpoint::disconnect()
    {
        std::scoped_lock lck(this->socket_mtx_);
        if (this->socket_)
        {
            std::string endpoint = this->get_last_address();
            logf_debug("%s disconnecting from %s", *this, endpoint);
            this->try_or_log(
                ::zmq_disconnect(this->socket_, endpoint.c_str()),
                "could not disconnect from " + endpoint);
        }
    }

    void Endpoint::initialize()
    {
        Super::initialize();
        this->open_socket();

        // this->setsockopt(ZMQ_IPV6, 1);
        // this->setsockopt(ZMQ_LINGER, 0);

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

        core::platform::signal_shutdown.connect(
            this->deinit_signal_handle_,
            std::bind(&This::deinitialize, this));
        //this->setsockopt(ZMQ_RCVTIMEO, 500);
    }

    void Endpoint::deinitialize()
    {
        core::platform::signal_shutdown.disconnect(
            this->deinit_signal_handle_);

        if (this->initialized())
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

            this->close_socket();
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

    std::string Endpoint::get_last_address() const
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
            return this->address();
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

    void Endpoint::setsockopt(int option, int value)
    {
        this->setsockopt(option, &value, sizeof(value));
    }

    void Endpoint::setsockopt(int option, const void *data, std::size_t data_size)
    {
        std::scoped_lock lck(this->socket_mtx_);
        if (this->socket_)
        {
            auto bytes = core::types::ByteVector::from_pointer(data, data_size);
            logf_trace("%s applying socket option %s, bytes: %s", *this, option, bytes.to_hex());
            this->check_error(::zmq_setsockopt(
                this->socket(),
                option,
                &data,
                data_size));
            logf_trace("%s applied socket option %s", *this, option);
        }
    }

    void Endpoint::send(const types::ByteVector &bytes, SendFlags flags) const
    {
        // using SendFunction = int (*)(void *socket, const void *buf, size_t len, int flags);
        // SendFunction send = (flags & ZMQ_DONTWAIT) ? ::zmq_send : ::zmq_send_const;

        logf_trace("%s sending %d bytes", *this, bytes.size());
        this->check_error(::zmq_send(
            this->socket(),  // socket
            bytes.data(),    // buf
            bytes.size(),    // len
            flags));         // flags
    }

    std::shared_ptr<types::ByteVector> Endpoint::receive(RecvFlags flags) const
    {
        std::vector<types::ByteVector> parts;
        if (std::size_t size = this->receive(&parts, flags))
        {
            auto bytes = std::make_shared<types::ByteVector>();
            bytes->reserve(size);

            for (const types::ByteVector &part : parts)
            {
                bytes->insert(bytes->end(), part.begin(), part.end());
            }
            return bytes;
        }
        else
        {
            return {};
        }
    }

    std::vector<types::ByteVector> Endpoint::receive_parts(RecvFlags flags) const
    {
        std::vector<types::ByteVector> parts;
        this->receive(&parts, flags);
        return parts;
    }

    std::size_t Endpoint::receive(std::vector<types::ByteVector> *parts, RecvFlags flags) const
    {
        zmq_msg_t msg;
        std::vector<std::string> counts;
        std::size_t total = 0;

        do
        {
            this->check_error(::zmq_msg_init(&msg));
            this->check_error(::zmq_msg_recv(&msg, this->socket(), flags));

            const std::uint8_t *data = static_cast<const std::uint8_t *>(::zmq_msg_data(&msg));
            size_t size = ::zmq_msg_size(&msg);
            total += size;

            parts->emplace_back(data, data + size);
            this->check_error(::zmq_msg_close(&msg));

            counts.push_back(std::to_string(size));
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

    Context *Endpoint::context_ = nullptr;

}  // namespace core::zmq
