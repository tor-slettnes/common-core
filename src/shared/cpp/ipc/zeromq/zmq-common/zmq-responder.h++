/// -*- c++ -*-
//==============================================================================
/// @file zmq-responder.h++
/// @brief Implements ZeroMQ request/reply pattern - responder
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-host.h++"
#include "types/bytearray.h++"
#include <thread>

namespace cc::zmq
{
    class Responder : public Host
    {
        using This = Responder;
        using Super = Host;

    protected:
        Responder(const std::string &bind_address,
                  const std::string &class_name,
                  const std::string &channel_name);

        std::vector<std::string> settings_path() const override;

    public:
        void start();
        void stop();
        void run();

    protected:
        // Subclasses should ovreride either of the following methods to
        // handle service requests.
        virtual void process_zmq_request(const ::zmq::message_t &request_msg,
                                         types::ByteArray *packed_reply);

        virtual void process_binary_request(const types::ByteArray &packed_request,
                                            types::ByteArray *packed_reply);

    private:
        std::thread listen_thread;
        bool keep_listening;
    };

}  // namespace cc::zmq
