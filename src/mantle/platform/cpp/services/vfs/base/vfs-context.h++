// -*- c++ -*-
//==============================================================================
/// @file vfs-context.h++
/// @brief VFS service - abstract file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-types.h++"
#include "types/shared_ptr_map.h++"

#include <unordered_map>

namespace platform::vfs
{
    //==========================================================================
    /// \class Context
    /// \brief VFS location context.

    class Context : public core::types::Streamable
    {
    public:
        using ptr = std::shared_ptr<Context>;

    public:
        Context(const ContextName &name = {},
                const fs::path &root = {},
                bool writable = false,
                bool removable = false,
                const std::string title = {});

        virtual void add_ref() = 0;
        virtual void del_ref() = 0;

        virtual fs::path localPath(const fs::path &relpath = {}) const;
        virtual Path virtualPath(const fs::path &relpath = {}) const;

    protected:
        virtual void to_stream(std::ostream &stream) const override;

    private:
        void check_jail(const fs::path &relpath) const;

    public:
        ContextName name;
        fs::path root;
        bool writable;
        bool removable;
        std::string title;
    };

    // using ContextMap = core::types::shared_ptr_map<
    //     ContextName,
    //     Context,
    //     std::unordered_map<ContextName, std::shared_ptr<Context>>>;

    using ContextMap = std::unordered_map<ContextName, std::shared_ptr<Context>>;

    using ContextList = std::vector<Context::ptr>;

}  // namespace platform::vfs
