/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-messagebase.h++
/// @brief Mix-in base for ProtoBuf message types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-filter.h++"

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

        Filter default_filter()
        {
            return Filter::create_from_topic(this->type_name());
        }

        std::string type_name()
        {
            return this->type_name_;
        }

    private:
        std::string type_name_;
    };

}  // namespace core::zmq
