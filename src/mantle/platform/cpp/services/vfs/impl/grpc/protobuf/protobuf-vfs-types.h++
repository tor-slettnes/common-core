/// -*- c++ -*-
//==============================================================================
/// @file protobuf-vfs-types.h++
/// @brief Encode/decode routines for vfs ProtoBuf types
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/vfs/protobuf/vfs_types.pb.h"  // generated from `variant.proto`
#include "vfs-base.h++"

namespace cc::protobuf
{
    //==========================================================================
    // Operation Flags

    void encode(const platform::vfs::OperationFlags &flags,
                platform::vfs::protobuf::PathRequest *msg);

    void decode(const platform::vfs::protobuf::PathRequest &msg,
                platform::vfs::OperationFlags *flags);

    //==========================================================================
    // PathType

    void encode(fs::file_type type,
                platform::vfs::protobuf::PathType *prototype);

    void decode(platform::vfs::protobuf::PathType prototype,
                fs::file_type *type);

    //==========================================================================
    // ContextSpec

    void encode(const platform::vfs::Context &spec,
                platform::vfs::protobuf::ContextSpec *msg);

    void decode(const platform::vfs::protobuf::ContextSpec &msg,
                platform::vfs::Context *ref);

    //==========================================================================
    // ContextMap

    void encode(const platform::vfs::ContextMap &list,
                platform::vfs::protobuf::ContextMap *msg);

    //==========================================================================
    // Path

    void encode(const platform::vfs::Path &vpath,
                platform::vfs::protobuf::Path *msg);

    void decode(const platform::vfs::protobuf::Path &msg,
                platform::vfs::Path *vpath);

    //==========================================================================
    // PathRequest

    void encode(const platform::vfs::Path &path,
                const platform::vfs::OperationFlags &flags,
                platform::vfs::protobuf::PathRequest *msg);

    void encode(const platform::vfs::Paths &sources,
                const platform::vfs::Path &target,
                const platform::vfs::OperationFlags &flags,
                platform::vfs::protobuf::PathRequest *msg);

    void decode(const platform::vfs::protobuf::PathRequest &msg,
                platform::vfs::Path *path,
                platform::vfs::OperationFlags *flags);

    void decode(const platform::vfs::protobuf::PathRequest &msg,
                platform::vfs::Paths *sources,
                platform::vfs::Path *target,
                platform::vfs::OperationFlags *flags);

    //==========================================================================
    // LocateRequest

    void encode(const platform::vfs::Path &root,
                const core::types::PathList &filename_masks,
                const core::types::TaggedValueList &attribute_filters,
                const platform::vfs::OperationFlags &flags,
                platform::vfs::protobuf::LocateRequest *msg);

    void decode(const platform::vfs::protobuf::LocateRequest &msg,
                platform::vfs::Path *root,
                core::types::PathList *filename_masks,
                core::types::TaggedValueList *attribute_filters,
                platform::vfs::OperationFlags *flags);

    //==========================================================================
    // AttributeRequest

    void encode(const platform::vfs::Path &vpath,
                const core::types::KeyValueMap &attributes,
                platform::vfs::protobuf::AttributeRequest *msg);

    void decode(const platform::vfs::protobuf::AttributeRequest &msg,
                platform::vfs::Path *vpath,
                core::types::KeyValueMap *attributes);

    //==========================================================================
    // VolumeInfo

    void encode(const platform::vfs::VolumeInfo &stats,
                platform::vfs::protobuf::VolumeInfo *msg);

    void decode(const platform::vfs::protobuf::VolumeInfo &msg,
                platform::vfs::VolumeInfo *stats);

    //==========================================================================
    // FileInfo

    void encode(const platform::vfs::FileInfo &stats,
                platform::vfs::protobuf::FileInfo *msg);

    void encode(const std::string &name,
                const platform::vfs::FileInfo &stats,
                platform::vfs::protobuf::FileInfo *msg);

    void decode(const platform::vfs::protobuf::FileInfo &msg,
                platform::vfs::FileInfo *stats);

    //==========================================================================
    // Directory

    void encode(const platform::vfs::Directory &dir,
                platform::vfs::protobuf::Directory *msg);

    void decode(const platform::vfs::protobuf::Directory &msg,
                platform::vfs::Directory *dir);

}  // namespace cc::protobuf
