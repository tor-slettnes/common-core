/// -*- c++ -*-
//==============================================================================
/// @file zmq-responder.h++
/// @brief Implements ZeroMQ request/reply pattern - responder
/// @author Tor Slettnes
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

        ~Responder();

    public:
        void start();
        void stop();
        void run();

    protected:
        virtual void process_binary_request(const types::ByteVector &packed_request,
                                            types::ByteVector *packed_reply) = 0;

    private:
        bool keep_listening;
        std::thread listen_thread;
    };

}  // namespace core::zmq
