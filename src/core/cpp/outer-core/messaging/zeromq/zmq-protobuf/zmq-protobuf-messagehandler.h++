/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-messagehandler.h++
/// @brief Handle received messages with a ProtoBuf payload
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-messagehandler.h++"
#include "zmq-protobuf-messagebase.h++"
#include "protobuf-message.h++"
#include "logging/logging.h++"

namespace core::zmq
{
    template <class ProtoT>
    class ProtoBufMessageHandler : public ProtoBufMessageBase<ProtoT>,
                                   public MessageHandler
    {
    protected:
        ProtoBufMessageHandler(const Filter &filter = {})
            : ProtoBufMessageBase<ProtoT>(),
              MessageHandler(this->type_name(),
                             filter.empty() ? this->default_filter() : filter)
        {
        }

    private:
        void handle(const types::ByteVector &bytes) override
        {
            ProtoT message = ::protobuf::to_message<ProtoT>(bytes);
            log_trace("ProtoBufMessageHandler() handling message: ", message);
            this->handle_message(message);
        }

    protected:
        virtual void handle_message(const ProtoT &message) = 0;
    };

}  // namespace core::zmq
