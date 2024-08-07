/// -*- c++ -*-
//==============================================================================
/// @file protobuf-vfs.h++
/// @brief Encode/decode routines for vfs ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs.pb.h"  // generated from `variant.proto`
#include "vfs-base.h++"

namespace protobuf
{
    //==========================================================================
    // Operation Flags

    void encode(const ::platform::vfs::OperationFlags &flags,
                ::cc::platform::vfs::PathRequest *msg);

    void decode(const ::cc::platform::vfs::PathRequest &msg,
                ::platform::vfs::OperationFlags *flags);

    //==========================================================================
    // PathType

    void encode(fs::file_type type,
                ::cc::platform::vfs::PathType *prototype);

    void decode(::cc::platform::vfs::PathType prototype,
                fs::file_type *type);

    //==========================================================================
    // ContextSpec

    void encode(const ::platform::vfs::Context &spec,
                ::cc::platform::vfs::ContextSpec *msg);

    void decode(const ::cc::platform::vfs::ContextSpec &msg,
                ::platform::vfs::Context *ref);

    //==========================================================================
    // ContextMap

    void encode(const ::platform::vfs::ContextMap &list,
                ::cc::platform::vfs::ContextMap *msg);
    void decode(const ::cc::platform::vfs::ContextMap &msg,
                ::platform::vfs::ContextMap *list);

    //==========================================================================
    // Path

    void encode(const ::platform::vfs::Path &vpath,
                ::cc::platform::vfs::Path *msg);

    void decode(const ::cc::platform::vfs::Path &msg,
                ::platform::vfs::Path *vpath);

    //==========================================================================
    // PathRequest

    void encode(const ::platform::vfs::Path &source,
                const ::platform::vfs::Path &target,
                const ::platform::vfs::OperationFlags &flags,
                ::cc::platform::vfs::PathRequest *msg);

    void encode(const ::platform::vfs::Paths &sources,
                const ::platform::vfs::Path &target,
                const ::platform::vfs::OperationFlags &flags,
                ::cc::platform::vfs::PathRequest *msg);

    void decode(const ::cc::platform::vfs::PathRequest &msg,
                ::platform::vfs::Path *source,
                ::platform::vfs::Path *target,
                ::platform::vfs::OperationFlags *flags);

    void decode(const ::cc::platform::vfs::PathRequest &msg,
                ::platform::vfs::Paths *sources,
                ::platform::vfs::Path *target,
                ::platform::vfs::OperationFlags *flags);

    //==========================================================================
    // LocateRequest

    void encode(const ::platform::vfs::Path &root,
                const std::vector<std::string> &filename_masks,
                const core::types::TaggedValueList &attribute_filters,
                const ::platform::vfs::OperationFlags &flags,
                ::cc::platform::vfs::LocateRequest *msg);

    void decode(const ::cc::platform::vfs::LocateRequest &msg,
                ::platform::vfs::Path *root,
                std::vector<std::string> *filename_masks,
                core::types::TaggedValueList *attribute_filters,
                ::platform::vfs::OperationFlags *flags);

    //==========================================================================
    // AttributeRequest

    void encode(const ::platform::vfs::Path &vpath,
                const core::types::KeyValueMap &attributes,
                ::cc::platform::vfs::AttributeRequest *msg);

    void decode(const ::cc::platform::vfs::AttributeRequest &msg,
                ::platform::vfs::Path *vpath,
                core::types::KeyValueMap *attributes);

    //==========================================================================
    // VolumeStats

    void encode(const ::platform::vfs::VolumeStats &stats,
                ::cc::platform::vfs::VolumeStats *msg);

    void decode(const ::cc::platform::vfs::VolumeStats &msg,
                ::platform::vfs::VolumeStats *stats);

    //==========================================================================
    // FileStats

    void encode(const ::platform::vfs::FileStats &stats,
                ::cc::platform::vfs::FileStats *msg);

    void encode(const std::string &name,
                const ::platform::vfs::FileStats &stats,
                ::cc::platform::vfs::FileStats *msg);

    void decode(const ::cc::platform::vfs::FileStats &msg,
                ::platform::vfs::FileStats *stats);

    //==========================================================================
    // Directory

    void encode(const ::platform::vfs::Directory &dir,
                ::cc::platform::vfs::Directory *msg);

    void decode(const ::cc::platform::vfs::Directory &msg,
                ::platform::vfs::Directory *dir);

    //==========================================================================
    // DirectoryList (legacy API)

    void encode(const ::platform::vfs::Directory &dir,
                ::cc::platform::vfs::DirectoryList *msg);

    void decode(const ::cc::platform::vfs::DirectoryList &msg,
                ::platform::vfs::Directory *dir);
}  // namespace protobuf
