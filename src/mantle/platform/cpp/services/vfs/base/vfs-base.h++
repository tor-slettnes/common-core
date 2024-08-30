// -*- c++ -*-
//==============================================================================
/// @file vfs-base.h++
/// @brief VFS service - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-types.h++"
#include "vfs-context.h++"
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
    public:
        virtual ContextMap get_contexts() const = 0;

        virtual ContextMap get_open_context() const = 0;

        virtual ContextRef get_context(const std::string &name, bool required = true) const = 0;

        virtual ContextRef open_context(const std::string &name, bool required = true) = 0;

        virtual void close_context(const ContextRef &cxt) = 0;

        virtual VolumeStats volume_stats(const Path &vpath,
                                         const OperationFlags &flags) const = 0;

        virtual FileStats file_stats(const Path &vpath,
                                     const OperationFlags &flags) const = 0;

        virtual Directory get_directory(const Path &vpath,
                                        const OperationFlags &flags) const = 0;

        virtual Directory locate(const Path &vpath,
                                 const std::vector<std::string> &filename_masks,
                                 const core::types::TaggedValueList &attribute_filters,
                                 const OperationFlags &flags) const = 0;

        virtual void copy(const Paths &sources,
                          const Path &target,
                          const OperationFlags &flags) const = 0;

        virtual void move(const Paths &sources,
                          const Path &target,
                          const OperationFlags &flags) const = 0;

        virtual void remove(const Paths &sources,
                            const OperationFlags &flags) const = 0;

        virtual void create_folder(const Path &vpath,
                                   const OperationFlags &flags) const = 0;

        virtual ReaderRef read_file(const Path &vpath) const = 0;

        virtual WriterRef write_file(const Path &vpath) const = 0;

        virtual core::types::KeyValueMap get_attributes(const Path &vpath) const = 0;

        virtual void set_attributes(const Path &vpath,
                                    const core::types::KeyValueMap &attributes) const = 0;

        virtual void clear_attributes(const Path &vpath) const = 0;
    };

    //==========================================================================
    // Provider instance

    extern core::platform::ProviderProxy<ProviderInterface> vfs;

}  // namespace platform::vfs
