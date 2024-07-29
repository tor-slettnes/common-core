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
        virtual ContextMap getContexts() const = 0;

        virtual ContextMap getOpenContexts() const = 0;

        virtual ContextRef getContext(const std::string &name, bool required = true) const = 0;

        virtual ContextRef openContext(const std::string &name, bool required = true) = 0;

        virtual void closeContext(const ContextRef &cxt) = 0;

        virtual VolumeStats volumeStats(const Path &vpath,
                                        const OperationFlags &flags) const = 0;

        virtual FileStats fileStats(const Path &vpath,
                                    const OperationFlags &flags) const = 0;

        virtual Directory getDirectory(const Path &vpath,
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

        virtual void createFolder(const Path &vpath,
                                  const OperationFlags &flags) const = 0;

        virtual ReaderRef readFile(const Path &vpath) const = 0;

        virtual WriterRef writeFile(const Path &vpath) const = 0;

        virtual core::types::KeyValueMap getAttributes(const Path &vpath) const = 0;

        virtual void setAttributes(const Path &vpath,
                                   const core::types::KeyValueMap &attributes) const = 0;

        virtual void clearAttributes(const Path &vpath) const = 0;
    };

    //==========================================================================
    // Provider instance

    extern core::platform::ProviderProxy<ProviderInterface> vfs;

}
