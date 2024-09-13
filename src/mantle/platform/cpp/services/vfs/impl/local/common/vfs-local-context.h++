// -*- c++ -*-
//==============================================================================
/// @file vfs-local-context.h++
/// @brief VFS service - local filesystem context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-context.h++"

#include <mutex>

namespace platform::vfs::local
{
    //==========================================================================
    // LocalContext

    class LocalContext : public Context,
                         public std::enable_shared_from_this<LocalContext>
    {
    public:
        LocalContext(const std::string &name,
                     const fs::path &root,
                     bool writable,
                     const std::string &preexec,
                     const std::string &postexec,
                     const std::string &title,
                     bool removable = false);

        void add_ref() override;
        void del_ref() override;

        void open();
        void close();

    protected:
        virtual void on_open();
        virtual void on_close();

    public:
        std::string preexec;
        std::string postexec;
        std::uint64_t refcount;

    private:
        std::mutex mtx;
    };
}  // namespace platform::vfs::local
