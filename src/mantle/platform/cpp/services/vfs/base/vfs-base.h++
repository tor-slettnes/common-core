// -*- c++ -*-
//==============================================================================
/// @file vfs-base.h++
/// @brief VFS service - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-types.h++"
#include "vfs-context.h++"
#include "vfs-location.h++"
#include "vfs-signals.h++"

namespace platform::vfs
{
    //==========================================================================
    // Provider

    class ProviderInterface : public core::platform::Provider
    {
        using This = ProviderInterface;
        using Super = core::platform::Provider;

    protected:
        using Super::Super;

    public:
        ContextProxy context_proxy(
            const std::string &name,
            bool modify) const;

        ContextProxy context_proxy(
            const Context::ptr &context,
            bool modify) const;

        Location location(
            const Path &path,
            bool modify) const;

        Location location(
            const ContextName &context,
            const fs::path &relpath,
            bool modify) const;

        Location location(
            const Context::ptr &context,
            const fs::path &relpath,
            bool modify) const;

        LocationList locations(
            const Paths &vpaths,
            bool modify) const;


    public:
        std::optional<std::string> read_chunk(
            std::istream &stream);

        std::streamsize write_chunk(
            std::ostream &stream,
            const std::string &chunk);


    public:
        virtual ContextMap get_contexts() const = 0;

        virtual ContextMap get_open_contexts() const = 0;

        virtual Context::ptr get_context(
            const std::string &name,
            bool required = true) const = 0;

        virtual Context::ptr open_context(
            const std::string &name,
            bool required = true) = 0;

        virtual void close_context(
            const std::string &name,
            bool required = false) = 0;

        virtual void close_context(
            const Context::ptr &cxt) = 0;

        virtual VolumeInfo get_volume_info(
            const Path &vpath,
            const OperationFlags &flags) const = 0;

        virtual FileInfo get_file_info(
            const Path &vpath,
            const OperationFlags &flags) const = 0;

        virtual Directory get_directory(
            const Path &vpath,
            const OperationFlags &flags) const = 0;

        virtual Directory locate(
            const Path &vpath,
            const core::types::PathList &filename_masks,
            const core::types::TaggedValueList &attribute_filters,
            const OperationFlags &flags) const = 0;

        virtual void copy(
            const Paths &sources,
            const Path &target,
            const OperationFlags &flags) const = 0;

        virtual void move(
            const Paths &sources,
            const Path &target,
            const OperationFlags &flags) const = 0;

        virtual void remove(
            const Paths &vpaths,
            const OperationFlags &flags) const = 0;

        virtual void create_folder(
            const Path &vpath,
            const OperationFlags &flags) const = 0;

        virtual UniqueReader read_file(
            const Path &vpath) const = 0;

        virtual UniqueWriter write_file(
            const Path &vpath) const = 0;

        virtual core::types::KeyValueMap get_attributes(
            const Path &vpath) const = 0;

        virtual void set_attributes(
            const Path &vpath,
            const core::types::KeyValueMap &attributes) const = 0;

        virtual void clear_attributes(
            const Path &vpath) const = 0;
    };

    //==========================================================================
    // Provider instance

    extern core::platform::ProviderProxy<ProviderInterface> vfs;

}  // namespace platform::vfs
