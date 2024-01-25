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

namespace cc::zmq
{
    Base::Base(const std::string &class_name,
               const std::string &channel_name,
               ::zmq::socket_type socket_type)
        : Super(class_name, channel_name),
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
                               const std::string &personality,
                               const types::Value &defaultValue) const
    {
        types::ValueList candidate_branches;

        if (!personality.empty())
        {
            candidate_branches.push_back(
                this->settings()->get(PERSONALITY_SECTION).get(personality, {}));
        }

        candidate_branches.push_back(
            this->settings()->get(DEFAULT_SECTION).get(this->channel_name()));

        for (types::Value &branch : candidate_branches)
        {
            types::Value value = branch.get(key);

            for (const std::string &sub_key : this->settings_path())
            {
                if (branch)
                {
                    value = branch.get(key, value);
                    branch = branch.get(sub_key);
                }
            }

            if (value)
            {
                return value;
            }
        }
        return defaultValue;
    }

    void Base::to_stream(std::ostream &stream) const
    {
        str::format(stream,
                    "ZMQ %s %s(%r)",
                    this->kind(),
                    this->class_name(),
                    this->channel_name());
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
            this->socket()->close();
        }
        catch (const std::exception &e)
        {
            logf_info("Could not close ZMQ host %s socket: %s",
                      *this,
                      e.what());
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
        std::string personality;

        this->splitaddress(address, &protocol, &host, &port, &personality);

        if (protocol.empty())
        {
            protocol = this->setting(protocolOption,
                                     personality,
                                     defaultProtocol)
                           .as_string();
        }

        if (host.empty())
        {
            host = this->setting(hostOption,
                                 personality,
                                 defaultHost)
                       .as_string();
        }

        if (port == 0)
        {
            port = this->setting(portOption,
                                 personality,
                                 defaultPort)
                       .as_uint();
        }

        return this->joinaddress(protocol, host, port);
    }

    void Base::splitaddress(const std::string &address,
                            std::string *protocol,
                            std::string *host,
                            uint *port,
                            std::string *personality) const
    {
        static const std::regex rx(
            "(?:(\\w*)://)?"                       // protocol
            "(\\*|\\[[\\w\\.:]*\\]|[\\w\\-\\.]*)"  // host, either '[x[:x...]]' or 'n[.n]...'
            "(?::(\\d+))?"                         // port
            "(?:(\\w*)@)?"                         // personality
        );

        std::smatch match;
        if (std::regex_match(address, match, rx))
        {
            *protocol = match.str(1);
            *host = match.str(2);
            *port = match.length(3) ? std::stoi(match.str(3)) : 0;
            *personality = match.str(4);
        }
        else
        {
            protocol->clear();
            host->clear();
            *port = 0;
            personality->clear();
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

    std::string Base::kind() const
    {
        return str::join(this->settings_path());
    }

    std::mutex Base::context_mtx_;

    std::shared_ptr<::zmq::context_t> Base::context_;

    std::shared_ptr<SettingsStore> Base::settings_;

}  // namespace cc::zmq
