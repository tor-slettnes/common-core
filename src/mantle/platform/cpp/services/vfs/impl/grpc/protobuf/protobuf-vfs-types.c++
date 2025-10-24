/// -*- c++ -*-
//==============================================================================
/// @file protobuf-vfs-types.c++
/// @brief Encode/decode routines for vfs ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-vfs-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"
#include "status/exceptions.h++"
#include "types/filesystem.h++"

#include <unordered_map>

namespace protobuf
{
    //==========================================================================
    // Operation Flags

    void encode(const ::vfs::OperationFlags &flags,
                ::cc::platform::vfs::PathRequest *msg)
    {
        msg->set_force(flags.force);
        msg->set_dereference(flags.dereference);
        msg->set_merge(flags.merge);
        msg->set_update(flags.update);
        msg->set_with_attributes(flags.with_attributes);
        msg->set_include_hidden(flags.include_hidden);
        msg->set_inside_target(flags.inside_target);
    }

    void decode(const ::cc::platform::vfs::PathRequest &msg,
                ::vfs::OperationFlags *flags)
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
                              ::cc::platform::vfs::PathType>
        type_map = {
            {fs::file_type::none, ::cc::platform::vfs::TYPE_NONE},
            {fs::file_type::not_found, ::cc::platform::vfs::TYPE_NONE},
            {fs::file_type::regular, ::cc::platform::vfs::TYPE_FILE},
            {fs::file_type::directory, ::cc::platform::vfs::TYPE_DIRECTORY},
            {fs::file_type::symlink, ::cc::platform::vfs::TYPE_SYMLINK},
            {fs::file_type::character, ::cc::platform::vfs::TYPE_CHARDEV},
            {fs::file_type::block, ::cc::platform::vfs::TYPE_BLOCKDEV},
            {fs::file_type::fifo, ::cc::platform::vfs::TYPE_PIPE},
            {fs::file_type::socket, ::cc::platform::vfs::TYPE_SOCKET}};

    void encode(fs::file_type type,
                ::cc::platform::vfs::PathType *prototype)
    {
        try
        {
            *prototype = type_map.at(type);
        }
        catch (const std::out_of_range &)
        {
            *prototype = ::cc::platform::vfs::TYPE_NONE;
        }
    }

    void decode(::cc::platform::vfs::PathType prototype,
                fs::file_type *type)
    {
        *type = fs::file_type::none;
        for (const auto &[candidate, value] : type_map)
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

    void encode(const ::vfs::Context &spec,
                ::cc::platform::vfs::ContextSpec *msg)
    {
        msg->set_name(spec.name);
        msg->set_root(spec.root.string());
        msg->set_writable(spec.writable);
        msg->set_removable(spec.removable);
        msg->set_title(spec.title);
    }

    void decode(const ::cc::platform::vfs::ContextSpec &msg,
                ::vfs::Context *ref)
    {
        ref->name = msg.name();
        ref->root = msg.root();
        ref->writable = msg.writable();
        ref->removable = msg.removable();
        ref->title = msg.title();
    }

    //==========================================================================
    // ContextMap

    void encode(const ::vfs::ContextMap &map,
                ::cc::platform::vfs::ContextMap *msg)
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[id, data] : map)
        {
            encode_shared(data, &(*encoded)[id]);
        }
    }

    //==========================================================================
    // Path

    void encode(const ::vfs::Path &vpath,
                ::cc::platform::vfs::Path *msg)
    {
        msg->set_context(vpath.context);
        msg->set_relpath(vpath.relpath);
    }

    void decode(const ::cc::platform::vfs::Path &msg,
                ::vfs::Path *vpath)
    {
        vpath->context = msg.context();
        vpath->relpath = msg.relpath();
    }

    //==========================================================================
    // PathRequest

    void encode(const ::vfs::Path &vpath,
                const ::vfs::OperationFlags &flags,
                ::cc::platform::vfs::PathRequest *msg)
    {
        encode(vpath, msg->mutable_path());
        encode(flags, msg);
    }

    void encode(const ::vfs::Paths &sources,
                const ::vfs::Path &target,
                const ::vfs::OperationFlags &flags,
                ::cc::platform::vfs::PathRequest *msg)
    {
        encode_vector(sources, msg->mutable_sources());
        encode(target, flags, msg);
    }

    void decode(const ::cc::platform::vfs::PathRequest &msg,
                ::vfs::Path *vpath,
                ::vfs::OperationFlags *flags)
    {
        decode(msg.path(), vpath);
        decode(msg, flags);
    }

    void decode(const ::cc::platform::vfs::PathRequest &msg,
                ::vfs::Paths *sources,
                ::vfs::Path *target,
                ::vfs::OperationFlags *flags)
    {
        decode_to_vector(msg.sources(), sources);
        decode(msg, target, flags);
    }

    //==========================================================================
    // LocateRequest

    void encode(const ::vfs::Path &root,
                const core::types::PathList &filename_masks,
                const core::types::TaggedValueList &attribute_filters,
                const ::vfs::OperationFlags &flags,
                ::cc::platform::vfs::LocateRequest *msg)
    {
        encode(root, msg->mutable_root());

        auto encoded_masks =  msg->mutable_filename_masks();
        encoded_masks->Reserve(filename_masks.size());
        for (const fs::path &mask: filename_masks)
        {
            encoded_masks->Add(mask.string());
        }

        encode(attribute_filters, msg->mutable_attribute_filters());
        msg->set_with_attributes(flags.with_attributes);
        msg->set_include_hidden(flags.include_hidden);
        msg->set_ignore_case(flags.ignore_case);
    }

    void decode(const ::cc::platform::vfs::LocateRequest &msg,
                ::vfs::Path *root,
                core::types::PathList *filename_masks,
                core::types::TaggedValueList *attribute_filters,
                ::vfs::OperationFlags *flags)
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

    void encode(const ::vfs::Path &vpath,
                const core::types::KeyValueMap &attributes,
                ::cc::platform::vfs::AttributeRequest *msg)
    {
        encode(vpath, msg->mutable_path());
        encode(attributes, msg->mutable_attributes());
    }

    void decode(const ::cc::platform::vfs::AttributeRequest &msg,
                ::vfs::Path *vpath,
                core::types::KeyValueMap *attributes)
    {
        decode(msg.path(), vpath);
        decode(msg.attributes(), attributes);
    }

    //==========================================================================
    // VolumeInfo

    void encode(const ::vfs::VolumeInfo &stats,
                ::cc::platform::vfs::VolumeInfo *msg)
    {
        msg->set_capacity(stats.capacity);
        msg->set_free(stats.free);
        msg->set_available(stats.available);
    }

    void decode(const ::cc::platform::vfs::VolumeInfo &msg,
                ::vfs::VolumeInfo *stats)
    {
        stats->capacity = msg.capacity();
        stats->free = msg.free();
        stats->available = msg.available();
    }

    //==========================================================================
    // FileInfo

    void encode(const ::vfs::FileInfo &stats,
                ::cc::platform::vfs::FileInfo *msg)
    {
        msg->set_type(encoded<::cc::platform::vfs::PathType>(stats.type));
        msg->set_size(stats.size);
        msg->set_mode(stats.mode);
        msg->set_readable(stats.readable);
        msg->set_writable(stats.writable);
        msg->set_link(stats.link.string());
        msg->set_uid(stats.uid);
        msg->set_gid(stats.gid);
        msg->set_ownername(stats.owner);
        msg->set_groupname(stats.group);
        protobuf::encode(stats.access_time, msg->mutable_access_time());
        protobuf::encode(stats.modify_time, msg->mutable_modify_time());
        protobuf::encode(stats.create_time, msg->mutable_create_time());
        protobuf::encode(stats.attributes, msg->mutable_attributes());
    }

    void encode(const std::string &name,
                const ::vfs::FileInfo &stats,
                ::cc::platform::vfs::FileInfo *msg)
    {
        msg->set_name(name);
        encode(stats, msg);
    }

    void decode(const ::cc::platform::vfs::FileInfo &msg,
                ::vfs::FileInfo *stats)
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
        protobuf::decode(msg.attributes(), &stats->attributes);
    }

    //==========================================================================
    // Directory

    void encode(const ::vfs::Directory &dir,
                ::cc::platform::vfs::Directory *msg)
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[filename, stats] : dir)
        {
            if (!filename.empty())
            {
                encode(stats, &(*encoded)[filename.string()]);
            }
        }
    }

    void decode(const ::cc::platform::vfs::Directory &msg,
                ::vfs::Directory *dir)
    {
        for (const auto &[filename, stats] : msg.map())
        {
            if (filename.size())
            {
                decode(stats, &(*dir)[filename]);
            }
        }
    }
}  // namespace protobuf
