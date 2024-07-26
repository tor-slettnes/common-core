/// -*- c++ -*-
//==============================================================================
/// @file protobuf-vfs.c++
/// @brief Encode/decode routines for vfs ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-vfs.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"
#include "vfs-remote-context.h++"
#include "status/exceptions.h++"

#include <unordered_map>

namespace protobuf
{
    //==========================================================================
    // Operation Flags

    void encode(const ::platform::vfs::OperationFlags &flags,
                ::cc::vfs::PathRequest *msg)
    {
        msg->set_force(flags.force);
        msg->set_dereference(flags.dereference);
        msg->set_merge(flags.merge);
        msg->set_update(flags.update);
        msg->set_with_attributes(flags.with_attributes);
        msg->set_include_hidden(flags.include_hidden);
        msg->set_inside_target(flags.inside_target);
    }

    void decode(const ::cc::vfs::PathRequest &msg,
                ::platform::vfs::OperationFlags *flags)
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
                              ::cc::vfs::PathType>
        type_map = {
            {fs::file_type::none, ::cc::vfs::TYPE_NONE},
            {fs::file_type::not_found, ::cc::vfs::TYPE_NONE},
            {fs::file_type::regular, ::cc::vfs::TYPE_FILE},
            {fs::file_type::directory, ::cc::vfs::TYPE_DIRECTORY},
            {fs::file_type::symlink, ::cc::vfs::TYPE_SYMLINK},
            {fs::file_type::character, ::cc::vfs::TYPE_CHARDEV},
            {fs::file_type::block, ::cc::vfs::TYPE_BLOCKDEV},
            {fs::file_type::fifo, ::cc::vfs::TYPE_PIPE},
            {fs::file_type::socket, ::cc::vfs::TYPE_SOCKET}};

    void encode(fs::file_type type,
                ::cc::vfs::PathType *prototype)
    {
        try
        {
            *prototype = type_map.at(type);
        }
        catch (const std::out_of_range &)
        {
            *prototype = ::cc::vfs::TYPE_NONE;
        }
    }

    void decode(::cc::vfs::PathType prototype,
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

    void encode(const ::platform::vfs::Context &spec,
                ::cc::vfs::ContextSpec *msg)
    {
        msg->set_name(spec.name);
        msg->set_root(spec.root.string());
        msg->set_writable(spec.writable);
        msg->set_removable(spec.removable);
        msg->set_title(spec.title);
    }

    void decode(const ::cc::vfs::ContextSpec &msg,
                ::platform::vfs::Context *ref)
    {
        ref->name = msg.name();
        ref->root = msg.root();
        ref->writable = msg.writable();
        ref->removable = msg.removable();
        ref->title = msg.title();
    }

    //==========================================================================
    // ContextMap

    void encode(const ::platform::vfs::ContextMap &map,
                ::cc::vfs::ContextMap *msg)
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[id, data] : map)
        {
            encode_shared(data, &(*encoded)[id]);
        }
    }

    void decode(const ::cc::vfs::ContextMap &msg,
                ::platform::vfs::ContextMap *map)
    {
        for (const auto &[id, data] : msg.map())
        {
            map->insert_or_assign(
                id,
                protobuf::decode_shared<::platform::vfs::RemoteContext>(data));
        }
    }

    //==========================================================================
    // Path

    void encode(const ::platform::vfs::Path &vpath,
                ::cc::vfs::Path *msg)
    {
        msg->set_context(vpath.context);
        msg->set_relpath(vpath.relpath);
    }

    void decode(const ::cc::vfs::Path &msg,
                ::platform::vfs::Path *vpath)
    {
        vpath->context = msg.context();
        vpath->relpath = msg.relpath();
    }

    //==========================================================================
    // PathRequest

    void encode(const ::platform::vfs::Path &source,
                const ::platform::vfs::Path &target,
                const ::platform::vfs::OperationFlags &flags,
                ::cc::vfs::PathRequest *msg)
    {
        encode(::platform::vfs::Paths({source}), target, flags, msg);
    }

    void encode(const ::platform::vfs::Paths &sources,
                const ::platform::vfs::Path &target,
                const ::platform::vfs::OperationFlags &flags,
                ::cc::vfs::PathRequest *msg)
    {
        for (const ::platform::vfs::Path &source : sources)
        {
            encode(source, msg->add_source());
        }
        encode(target, msg->mutable_target());
        encode(flags, msg);
    }

    void decode(const ::cc::vfs::PathRequest &msg,
                ::platform::vfs::Path *source,
                ::platform::vfs::Path *target,
                ::platform::vfs::OperationFlags *flags)
    {
        if (msg.source_size() != 1)
        {
            throw core::exception::InvalidArgument("Exactly one source path is required");
        }
        else
        {
            ::platform::vfs::Paths sources;
            decode(msg, &sources, target, flags);
            *source = sources.front();
        }
    }

    void decode(const ::cc::vfs::PathRequest &msg,
                ::platform::vfs::Paths *sources,
                ::platform::vfs::Path *target,
                ::platform::vfs::OperationFlags *flags)
    {
        sources->clear();
        for (const ::cc::vfs::Path &source : msg.source())
        {
            decode(source, &sources->emplace_back());
        }

        if (target != nullptr)
        {
            decode(msg.target(), target);
        }

        if (flags != nullptr)
        {
            decode(msg, flags);
        }
    }

    //==========================================================================
    // LocateRequest

    void encode(const ::platform::vfs::Path &root,
                const std::vector<std::string> &filename_masks,
                const core::types::TaggedValueList &attribute_filters,
                const ::platform::vfs::OperationFlags &flags,
                ::cc::vfs::LocateRequest *msg)
    {
        encode(root, msg->mutable_root());
        for (const std::string &mask : filename_masks)
        {
            msg->add_filename_mask(mask);
        }
        encode(attribute_filters, msg->mutable_attribute_filter());
        msg->set_with_attributes(flags.with_attributes);
        msg->set_include_hidden(flags.include_hidden);
        msg->set_ignore_case(flags.ignore_case);
    }

    void decode(const ::cc::vfs::LocateRequest &msg,
                ::platform::vfs::Path *root,
                std::vector<std::string> *filename_masks,
                core::types::TaggedValueList *attribute_filters,
                ::platform::vfs::OperationFlags *flags)
    {
        decode(msg.root(), root);
        filename_masks->clear();
        filename_masks->reserve(msg.filename_mask_size());
        for (const std::string &mask : msg.filename_mask())
        {
            filename_masks->push_back(mask);
        }
        decode(msg.attribute_filter(), attribute_filters);
        flags->with_attributes = msg.with_attributes();
        flags->include_hidden = msg.include_hidden();
        flags->ignore_case = msg.ignore_case();
    }

    //==========================================================================
    // AttributeRequest

    void encode(const ::platform::vfs::Path &vpath,
                const core::types::KeyValueMap &attributes,
                ::cc::vfs::AttributeRequest *msg)
    {
        encode(vpath, msg->mutable_path());
        for (auto it = attributes.begin(); it != attributes.end(); it++)
        {
            cc::variant::Value *tv = msg->add_attributes();
            tv->set_tag(it->first);
            encode(it->second, tv);
        }
    }

    void decode(const ::cc::vfs::AttributeRequest &msg,
                ::platform::vfs::Path *vpath,
                core::types::KeyValueMap *attributes)
    {
        decode(msg.path(), vpath);
        decode(msg.attributes(), attributes);
    }

    //==========================================================================
    // VolumeStats

    void encode(const ::platform::vfs::VolumeStats &stats,
                ::cc::vfs::VolumeStats *msg)
    {
        msg->set_capacity(stats.capacity);
        msg->set_free(stats.free);
        msg->set_available(stats.available);
    }

    void decode(const ::cc::vfs::VolumeStats &msg,
                ::platform::vfs::VolumeStats *stats)
    {
        stats->capacity = msg.capacity();
        stats->free = msg.free();
        stats->available = msg.available();
    }

    //==========================================================================
    // FileStats

    void encode(const ::platform::vfs::FileStats &stats,
                ::cc::vfs::FileStats *msg)
    {
        msg->set_type(encoded<::cc::vfs::PathType>(stats.type));
        msg->set_size(stats.size);
        msg->set_mode(stats.mode);
        msg->set_readable(stats.readable);
        msg->set_writable(stats.writable);
        msg->set_link(stats.link.string());
        msg->set_uid(stats.uid);
        msg->set_gid(stats.gid);
        msg->set_ownername(stats.owner);
        msg->set_groupname(stats.group);
        protobuf::encode(stats.accessTime, msg->mutable_accesstime());
        protobuf::encode(stats.modifyTime, msg->mutable_modifytime());
        protobuf::encode(stats.createTime, msg->mutable_createtime());

        for (auto it = stats.attributes.begin(); it != stats.attributes.end(); it++)
        {
            cc::variant::Value *tv = msg->add_attributes();
            tv->set_tag(it->first);
            encode(it->second, tv);
        }
    }

    void encode(const std::string &name,
                const ::platform::vfs::FileStats &stats,
                ::cc::vfs::FileStats *msg)
    {
        msg->set_name(name);
        encode(stats, msg);
    }

    void decode(const ::cc::vfs::FileStats &msg,
                ::platform::vfs::FileStats *stats)
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
        stats->accessTime = decoded<core::dt::TimePoint>(msg.accesstime());
        stats->modifyTime = decoded<core::dt::TimePoint>(msg.modifytime());
        stats->createTime = decoded<core::dt::TimePoint>(msg.createtime());
        protobuf::decode(msg.attributes(), &stats->attributes);
    }

    //==========================================================================
    // Directory

    void encode(const ::platform::vfs::Directory &dir,
                ::cc::vfs::Directory *msg)
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

    void decode(const ::cc::vfs::Directory &msg,
                ::platform::vfs::Directory *dir)
    {
        for (const auto &[filename, stats] : msg.map())
        {
            if (filename.size())
            {
                decode(stats, &(*dir)[filename]);
            }
        }
    }

    //==========================================================================
    // DirectoryList

    void encode(const ::platform::vfs::Directory &dir,
                ::cc::vfs::DirectoryList *msg)
    {
        for (const auto &[filename, stats] : dir)
        {
            encode(filename, stats, msg->add_entry());
        }
    }

    void decode(const ::cc::vfs::DirectoryList &msg,
                ::platform::vfs::Directory *dir)
    {
        for (const ::cc::vfs::FileStats &entry : msg.entry())
        {
            decode(entry, &(*dir)[entry.name()]);
        }
    }
}  // namespace protobuf
