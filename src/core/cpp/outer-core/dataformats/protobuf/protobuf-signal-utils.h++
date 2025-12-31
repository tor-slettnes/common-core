/// -*- c++ -*-
//==============================================================================
/// @file protobuf-signal-utils.h++
/// @brief Miscellaneous utilities for ProtoBuf `Signal` messages
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/valuemap.h++"
#include <google/protobuf/message.h>

namespace protobuf
{
    using DescriptorMap = core::types::ValueMap<
        std::string,
        const google::protobuf::Descriptor *>;

    template <class SignalT>
    DescriptorMap signal_descriptors()
    {
        DescriptorMap map;
        const google::protobuf::Descriptor *md = SignalT::GetDescriptor();
        int nfields = md->field_count();
        for (int i = 0; i < nfields; i++)
        {
            const google::protobuf::FieldDescriptor *fd = md->field(i);
            if (fd->containing_oneof() &&
                (fd->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE))
            {
                map.insert_or_assign(fd->name(), fd->message_type());
            }
        }
        return map;
    }

    template <class SignalT>
    std::vector<std::string> signal_names()
    {
        std::vector<std::string> names;
        const google::protobuf::Descriptor *md = SignalT::GetDescriptor();
        int nfields = md->field_count();
        for (int i = 0; i < nfields; i++)
        {
            const google::protobuf::FieldDescriptor *fd = md->field(i);
            if (fd->containing_oneof() &&
                (fd->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE))
            {
                names.push_back(fd->name());
            }
        }
        return names;
    }

}  // namespace protobuf
