/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-messagebase.h++
/// @brief Mix-in base for ProtoBuf message types
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/bytevector.h++"

namespace core::zmq
{
    template <class ProtoT>
    class ProtoBufMessageBase
    {
        using This = ProtoBufMessageBase<ProtoT>;

    public:
        ProtoBufMessageBase()
            : type_name_(ProtoT().GetTypeName())
        {
        }

        core::types::ByteVector default_filter()
        {
            return this->type_name();
        }

        std::string type_name()
        {
            return this->type_name_;
        }

    private:
        std::string type_name_;
    };

}  // namespace core::zmq
