/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-messagewriter.h++
/// @brief Send ProtoBuf messages with topic filter
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-protobuf-messagebase.h++"
#include "zmq-messagewriter.h++"
#include "protobuf-message.h++"

namespace core::zmq
{
    template <class ProtoT>
    class ProtoBufMessageWriter : public ProtoBufMessageBase<ProtoT>,
                                  public MessageWriter
    {
    protected:
        ProtoBufMessageWriter(const std::shared_ptr<Publisher> &publisher,
                              const std::optional<core::types::ByteVector> &header = {})
            : ProtoBufMessageBase<ProtoT>(),
              MessageWriter(publisher,
                            header.value_or(this->default_filter()))
        {
        }

    public:
        void write(const ProtoT &message)
        {
            MessageWriter::write(protobuf::to_bytes(message));
        }
    };

}  // namespace core::zmq
