/// -*- c++ -*-
//==============================================================================
/// @file protobuf-vfs-types.h++
/// @brief Encode/decode routines for vfs ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/protobuf/vfs/vfs.pb.h"  // generated from `variant.proto`
#include "vfs-base.h++"

namespace protobuf
{
    //==========================================================================
    // Operation Flags

    void encode(const ::vfs::OperationFlags &flags,
                ::cc::platform::vfs::PathRequest *msg);

    void decode(const ::cc::platform::vfs::PathRequest &msg,
                ::vfs::OperationFlags *flags);

    //==========================================================================
    // PathType

    void encode(fs::file_type type,
                ::cc::platform::vfs::PathType *prototype);

    void decode(::cc::platform::vfs::PathType prototype,
                fs::file_type *type);

    //==========================================================================
    // ContextSpec

    void encode(const ::vfs::Context &spec,
                ::cc::platform::vfs::ContextSpec *msg);

    void decode(const ::cc::platform::vfs::ContextSpec &msg,
                ::vfs::Context *ref);

    //==========================================================================
    // ContextMap

    void encode(const ::vfs::ContextMap &list,
                ::cc::platform::vfs::ContextMap *msg);

    //==========================================================================
    // Path

    void encode(const ::vfs::Path &vpath,
                ::cc::platform::vfs::Path *msg);

    void decode(const ::cc::platform::vfs::Path &msg,
                ::vfs::Path *vpath);

    //==========================================================================
    // PathRequest

    void encode(const ::vfs::Path &path,
                const ::vfs::OperationFlags &flags,
                ::cc::platform::vfs::PathRequest *msg);

    void encode(const ::vfs::Paths &sources,
                const ::vfs::Path &target,
                const ::vfs::OperationFlags &flags,
                ::cc::platform::vfs::PathRequest *msg);

    void decode(const ::cc::platform::vfs::PathRequest &msg,
                ::vfs::Path *path,
                ::vfs::OperationFlags *flags);

    void decode(const ::cc::platform::vfs::PathRequest &msg,
                ::vfs::Paths *sources,
                ::vfs::Path *target,
                ::vfs::OperationFlags *flags);

    //==========================================================================
    // LocateRequest

    void encode(const ::vfs::Path &root,
                const core::types::PathList &filename_masks,
                const core::types::TaggedValueList &attribute_filters,
                const ::vfs::OperationFlags &flags,
                ::cc::platform::vfs::LocateRequest *msg);

    void decode(const ::cc::platform::vfs::LocateRequest &msg,
                ::vfs::Path *root,
                core::types::PathList *filename_masks,
                core::types::TaggedValueList *attribute_filters,
                ::vfs::OperationFlags *flags);

    //==========================================================================
    // AttributeRequest

    void encode(const ::vfs::Path &vpath,
                const core::types::KeyValueMap &attributes,
                ::cc::platform::vfs::AttributeRequest *msg);

    void decode(const ::cc::platform::vfs::AttributeRequest &msg,
                ::vfs::Path *vpath,
                core::types::KeyValueMap *attributes);

    //==========================================================================
    // VolumeInfo

    void encode(const ::vfs::VolumeInfo &stats,
                ::cc::platform::vfs::VolumeInfo *msg);

    void decode(const ::cc::platform::vfs::VolumeInfo &msg,
                ::vfs::VolumeInfo *stats);

    //==========================================================================
    // FileInfo

    void encode(const ::vfs::FileInfo &stats,
                ::cc::platform::vfs::FileInfo *msg);

    void encode(const std::string &name,
                const ::vfs::FileInfo &stats,
                ::cc::platform::vfs::FileInfo *msg);

    void decode(const ::cc::platform::vfs::FileInfo &msg,
                ::vfs::FileInfo *stats);

    //==========================================================================
    // Directory

    void encode(const ::vfs::Directory &dir,
                ::cc::platform::vfs::Directory *msg);

    void decode(const ::cc::platform::vfs::Directory &msg,
                ::vfs::Directory *dir);

}  // namespace protobuf
