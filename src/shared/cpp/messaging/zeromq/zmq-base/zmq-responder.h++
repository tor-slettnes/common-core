/// -*- c++ -*-
//==============================================================================
/// @file zmq-responder.h++
/// @brief Implements ZeroMQ request/reply pattern - responder
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-host.h++"
#include "types/bytevector.h++"
#include <thread>

namespace shared::zmq
{
    class Responder : public Host
    {
        using This = Responder;
        using Super = Host;

    protected:
        Responder(const std::string &bind_address,
                  const std::string &channel_name);

    public:
        void start();
        void stop();
        void run();

    protected:
        virtual void process_binary_request(const types::ByteVector &packed_request,
                                            types::ByteVector *packed_reply) = 0;

    private:
        std::thread listen_thread;
        bool keep_listening;
    };

}  // namespace shared::zmq
