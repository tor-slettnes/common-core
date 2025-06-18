/// -*- c++ -*-
//==============================================================================
/// @file zmq-responder.h++
/// @brief Implements ZeroMQ request/reply pattern - responder
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-endpoint.h++"
#include "types/bytevector.h++"
#include <thread>

namespace core::zmq
{
    class Responder : public Endpoint
    {
        using This = Responder;
        using Super = Endpoint;

    protected:
        Responder(const std::string &address,
                  const std::string &channel_name,
                  Role role = Role::HOST);

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

}  // namespace core::zmq
