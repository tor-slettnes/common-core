/// -*- c++ -*-
//==============================================================================
/// @file zmq-responder.c++
/// @brief Implements ZeroMQ request/reply pattern - responder
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-responder.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"
#include "application/init.h++"

namespace cc::zmq
{
    Responder::Responder(const std::string &class_name,
                         const std::string &channel_name,
                         const std::string &bind_address)
        : Super(class_name, channel_name, ::zmq::socket_type::rep, bind_address),
          keep_listening(false)
    {
    }

    std::vector<std::string> Responder::settings_path() const
    {
        return {COMMAND_GROUP, "server"};
    }

    void Responder::start()
    {
        this->keep_listening = true;
        if (!this->listen_thread.joinable())
        {
            log_info("Starting ZMQ listener thread");
            this->listen_thread = std::thread(&Responder::run, this);
        }
    }

    void Responder::stop()
    {
        this->keep_listening = false;
        if (std::thread t{std::move(this->listen_thread)}; t.joinable())
        {
            log_info("Stopping ZMQ listener thread");
            t.join();
        }
    }

    void Responder::run()
    {
        this->keep_listening = true;
        try
        {
            while (this->keep_listening)
            {
                ::zmq::message_t msg;
                if (this->receive(&msg))
                {
                    types::ByteArray packed_reply;
                    this->process_zmq_request(msg, &packed_reply);
                    this->send(packed_reply);
                }
            }
        }
        catch (...)
        {
            logf_info("Shutting down ZMQ %s responder: %s",
                      this->channel_name(),
                      std::current_exception());
        }
    }

    void Responder::process_zmq_request(const ::zmq::message_t &request_msg,
                                        types::ByteArray *packed_reply)
    {
        // We get here only if the derived responder class does not override
        // this method. We assume that in this case, it _does_ override
        // the 2nd overload, which accepts a `ByteArray` input.

        const std::string_view &sv = request_msg.to_string_view();
        this->process_binary_request(types::ByteArray(sv.begin(), sv.end()),
                                     packed_reply);
    }

    void Responder::process_binary_request(const types::ByteArray &packed_request,
                                           types::ByteArray *packed_reply)
    {
    }

}  // namespace cc::zmq
