/// -*- c++ -*-
//==============================================================================
/// @file zmq-responder.c++
/// @brief Implements ZeroMQ request/reply pattern - responder
/// @author Tor Slettnes
//==============================================================================

#include "zmq-responder.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"
#include "platform/init.h++"

namespace core::zmq
{
    Responder::Responder(const std::string &address,
                         const std::string &channel_name,
                         Role role)
        : Super(address, "responder", channel_name, ZMQ_REP, role),
          keep_listening(false)
    {
    }

    Responder::~Responder()
    {
        this->stop();
        if (std::thread t{std::move(this->listen_thread)}; t.joinable())
        {
            t.detach();
        }
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
    }

    void Responder::run()
    {
        this->keep_listening = true;
        logf_trace("%s is listening for requests", *this);

        try
        {
            while (this->keep_listening)
            {
                if (auto request = this->receive())
                {
                    types::ByteVector reply;
                    this->process_binary_request(*request, &reply);
                    if (this->initialized())
                    {
                        this->send(reply);
                    }
                }
            }
        }
        catch (const Error &e)
        {
            if (this->keep_listening)
            {
                this->log_zmq_error("could not receive request", e);
            }
        }

        logf_trace("%s is no longer listening for requests", *this);
    }

}  // namespace core::zmq
