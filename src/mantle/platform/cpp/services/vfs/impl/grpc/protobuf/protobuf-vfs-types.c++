/// -*- c++ -*-
//==============================================================================
/// @file protobuf-vfs-types.c++
/// @brief Encode/decode routines for vfs ProtoBuf types
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-vfs-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"
#include "status/exceptions.h++"
#include "types/filesystem.h++"

#include <unordered_map>

namespace cc::protobuf
{
    //==========================================================================
    // Operation Flags

    void encode(const platform::vfs::OperationFlags& flags,
                platform::vfs::protobuf::PathRequest* msg)
    {
        msg->set_force(flags.force);
        msg->set_dereference(flags.dereference);
        msg->set_merge(flags.merge);
        msg->set_update(flags.update);
        msg->set_with_attributes(flags.with_attributes);
        msg->set_include_hidden(flags.include_hidden);
        msg->set_inside_target(flags.inside_target);
    }

    void decode(const platform::vfs::protobuf::PathRequest& msg,
                platform::vfs::OperationFlags* flags)
    {
        flags->force = msg.force();
        flags->dereference = msg.dereference();
        flags->merge = msg.merge();
        flags->update = msg.update();
        flags->with_attributes = msg.with_attributes();
        flags->include_hidden = msg.include_hidden();
        flags->inside_target = msg.inside_target();
    }

    //==========================================================================
    // PathType

    static std::unordered_map<fs::file_type,
                              platform::vfs::protobuf::PathType>
        type_map = {
            {fs::file_type::none, platform::vfs::protobuf::TYPE_NONE},
            {fs::file_type::not_found, platform::vfs::protobuf::TYPE_NONE},
            {fs::file_type::regular, platform::vfs::protobuf::TYPE_FILE},
            {fs::file_type::directory, platform::vfs::protobuf::TYPE_DIRECTORY},
            {fs::file_type::symlink, platform::vfs::protobuf::TYPE_SYMLINK},
            {fs::file_type::character, platform::vfs::protobuf::TYPE_CHARDEV},
            {fs::file_type::block, platform::vfs::protobuf::TYPE_BLOCKDEV},
            {fs::file_type::fifo, platform::vfs::protobuf::TYPE_PIPE},
            {fs::file_type::socket, platform::vfs::protobuf::TYPE_SOCKET}};

    void encode(fs::file_type type,
                platform::vfs::protobuf::PathType* prototype)
    {
        try
        {
            *prototype = type_map.at(type);
        }
        catch (const std::out_of_range&)
        {
            *prototype = platform::vfs::protobuf::TYPE_NONE;
        }
    }

    void decode(platform::vfs::protobuf::PathType prototype,
                fs::file_type* type)
    {
        *type = fs::file_type::none;
        for (const auto& [candidate, value] : type_map)
        {
            if (value == prototype)
            {
                *type = candidate;
                break;
            }
        }
    }

    //==========================================================================
    // ContextSpec

    void encode(const platform::vfs::Context& spec,
                platform::vfs::protobuf::ContextSpec* msg)
    {
        msg->set_name(spec.name);
        msg->set_root(spec.root.string());
        msg->set_writable(spec.writable);
        msg->set_removable(spec.removable);
        msg->set_title(spec.title);
    }

    void decode(const platform::vfs::protobuf::ContextSpec& msg,
                platform::vfs::Context* ref)
    {
        ref->name = msg.name();
        ref->root = msg.root();
        ref->writable = msg.writable();
        ref->removable = msg.removable();
        ref->title = msg.title();
    }

    //==========================================================================
    // ContextMap

    void encode(const platform::vfs::ContextMap& map,
                platform::vfs::protobuf::ContextMap* msg)
    {
        auto* encoded = msg->mutable_map();
        for (const auto& [id, data] : map)
        {
            encode_shared(data, &(*encoded)[id]);
        }
    }

    //==========================================================================
    // Path

    void encode(const platform::vfs::Path& vpath,
                platform::vfs::protobuf::Path* msg)
    {
        msg->set_context(vpath.context);
        msg->set_relpath(vpath.relpath);
    }

    void decode(const platform::vfs::protobuf::Path& msg,
                platform::vfs::Path* vpath)
    {
        vpath->context = msg.context();
        vpath->relpath = msg.relpath();
    }

    //==========================================================================
    // PathRequest

    void encode(const platform::vfs::Path& vpath,
                const platform::vfs::OperationFlags& flags,
                platform::vfs::protobuf::PathRequest* msg)
    {
        encode(vpath, msg->mutable_path());
        encode(flags, msg);
    }

    void encode(const platform::vfs::Paths& sources,
                const platform::vfs::Path& target,
                const platform::vfs::OperationFlags& flags,
                platform::vfs::protobuf::PathRequest* msg)
    {
        encode_vector(sources, msg->mutable_sources());
        encode(target, flags, msg);
    }

    void decode(const platform::vfs::protobuf::PathRequest& msg,
                platform::vfs::Path* vpath,
                platform::vfs::OperationFlags* flags)
    {
        decode(msg.path(), vpath);
        decode(msg, flags);
    }

    void decode(const platform::vfs::protobuf::PathRequest& msg,
                platform::vfs::Paths* sources,
                platform::vfs::Path* target,
                platform::vfs::OperationFlags* flags)
    {
        decode_to_vector(msg.sources(), sources);
        decode(msg, target, flags);
    }

    //==========================================================================
    // LocateRequest

    void encode(const platform::vfs::Path& root,
                const core::types::PathList& filename_masks,
                const core::types::TaggedValueList& attribute_filters,
                const platform::vfs::OperationFlags& flags,
                platform::vfs::protobuf::LocateRequest* msg)
    {
        encode(root, msg->mutable_root());

        auto encoded_masks = msg->mutable_filename_masks();
        encoded_masks->Reserve(filename_masks.size());
        for (const fs::path& mask : filename_masks)
        {
            encoded_masks->Add(mask.string());
        }

        encode(attribute_filters, msg->mutable_attribute_filters());
        msg->set_with_attributes(flags.with_attributes);
        msg->set_include_hidden(flags.include_hidden);
        msg->set_ignore_case(flags.ignore_case);
    }

    void decode(const platform::vfs::protobuf::LocateRequest& msg,
                platform::vfs::Path* root,
                core::types::PathList* filename_masks,
                core::types::TaggedValueList* attribute_filters,
                platform::vfs::OperationFlags* flags)
    {
        decode(msg.root(), root);

        filename_masks->assign(msg.filename_masks().begin(),
                               msg.filename_masks().end());

        decode(msg.attribute_filters(), attribute_filters);
        flags->with_attributes = msg.with_attributes();
        flags->include_hidden = msg.include_hidden();
        flags->ignore_case = msg.ignore_case();
    }

    //==========================================================================
    // AttributeRequest

    void encode(const platform::vfs::Path& vpath,
                const core::types::KeyValueMap& attributes,
                platform::vfs::protobuf::AttributeRequest* msg)
    {
        encode(vpath, msg->mutable_path());
        encode(attributes, msg->mutable_attributes());
    }

    void decode(const platform::vfs::protobuf::AttributeRequest& msg,
                platform::vfs::Path* vpath,
                core::types::KeyValueMap* attributes)
    {
        decode(msg.path(), vpath);
        decode(msg.attributes(), attributes);
    }

    //==========================================================================
    // VolumeInfo

    void encode(const platform::vfs::VolumeInfo& stats,
                platform::vfs::protobuf::VolumeInfo* msg)
    {
        msg->set_capacity(stats.capacity);
        msg->set_free(stats.free);
        msg->set_available(stats.available);
    }

    void decode(const platform::vfs::protobuf::VolumeInfo& msg,
                platform::vfs::VolumeInfo* stats)
    {
        stats->capacity = msg.capacity();
        stats->free = msg.free();
        stats->available = msg.available();
    }

    //==========================================================================
    // FileInfo

    void encode(const platform::vfs::FileInfo& stats,
                platform::vfs::protobuf::FileInfo* msg)
    {
        msg->set_type(encoded<platform::vfs::protobuf::PathType>(stats.type));
        msg->set_size(stats.size);
        msg->set_mode(stats.mode);
        msg->set_readable(stats.readable);
        msg->set_writable(stats.writable);
        msg->set_link(stats.link.string());
        msg->set_uid(stats.uid);
        msg->set_gid(stats.gid);
        msg->set_ownername(stats.owner);
        msg->set_groupname(stats.group);
        cc::protobuf::encode(stats.access_time, msg->mutable_access_time());
        cc::protobuf::encode(stats.modify_time, msg->mutable_modify_time());
        cc::protobuf::encode(stats.create_time, msg->mutable_create_time());
        cc::protobuf::encode(stats.attributes, msg->mutable_attributes());
    }

    void encode(const std::string& name,
                const platform::vfs::FileInfo& stats,
                platform::vfs::protobuf::FileInfo* msg)
    {
        msg->set_name(name);
        encode(stats, msg);
    }

    void decode(const platform::vfs::protobuf::FileInfo& msg,
                platform::vfs::FileInfo* stats)
    {
        stats->type = decoded<fs::file_type>(msg.type());
        stats->size = msg.size();
        stats->mode = msg.mode();
        stats->readable = msg.readable();
        stats->writable = msg.writable();
        stats->link = msg.link();
        stats->uid = msg.uid();
        stats->gid = msg.gid();
        stats->owner = msg.ownername();
        stats->group = msg.groupname();
        stats->access_time = decoded<core::dt::TimePoint>(msg.access_time());
        stats->modify_time = decoded<core::dt::TimePoint>(msg.modify_time());
        stats->create_time = decoded<core::dt::TimePoint>(msg.create_time());
        cc::protobuf::decode(msg.attributes(), &stats->attributes);
    }

    //==========================================================================
    // Directory

    void encode(const platform::vfs::Directory& dir,
                platform::vfs::protobuf::Directory* msg)
    {
        auto* encoded = msg->mutable_map();
        for (const auto& [filename, stats] : dir)
        {
            if (!filename.empty())
            {
                encode(stats, &(*encoded)[filename.string()]);
            }
        }
    }

    void decode(const platform::vfs::protobuf::Directory& msg,
                platform::vfs::Directory* dir)
    {
        for (const auto& [filename, stats] : msg.map())
        {
            if (filename.size())
            {
                decode(stats, &(*dir)[filename]);
            }
        }
    }
}  // namespace cc::protobuf
