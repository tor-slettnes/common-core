/// -*- c++ -*-
//==============================================================================
/// @file zmq-responder.c++
/// @brief Implements ZeroMQ request/reply pattern - responder
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-responder.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"
#include "platform/init.h++"

namespace cc::zmq
{
    Responder::Responder(const std::string &bind_address,
                         const std::string &channel_name)
        : Super(bind_address,
                "responder",
                channel_name,
                ::zmq::socket_type::rep),
          keep_listening(false)
    {
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
            log_debug("Waiting for ZMQ listener thread");
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
                if (auto request = this->receive())
                {
                    types::ByteVector reply;
                    this->process_binary_request(*request, &reply);
                    this->send(reply);
                }
            }
        }
        catch (const ::zmq::error_t &e)
        {
            this->log_zmq_error("could not continue receiving requests", e);
        }
    }

}  // namespace cc::zmq
