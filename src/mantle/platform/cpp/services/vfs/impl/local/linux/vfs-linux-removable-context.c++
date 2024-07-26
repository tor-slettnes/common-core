// -*- c++ -*-
//==============================================================================
/// @file vfs-linux-removable-context.c++
/// @brief VFS service - Removable context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-linux-removable-context.h++"
#include "platform/process.h++"

namespace platform::vfs::local
{
    RemovableContext::RemovableContext(
        const std::string &name,
        const fs::path &root,
        bool writable,
        const std::string &preexec,
        const std::string &postexec,
        const std::string &title,
        const std::string &devnode,
        const std::string &fstype)
        : Super(name, root, writable, preexec, postexec, title, true),
          devnode(devnode),
          fstype(fstype)
    {
        logf_debug("Added removable context %s", *this);
    }

    void RemovableContext::on_open()
    {
        Super::on_open();
        core::platform::ArgVector mountpoint = {
            "/bin/mountpoint",
            "-q",
            this->root,
        };

        if (core::platform::process->invoke_capture(mountpoint) != 0)
        {
            core::platform::ArgVector mount = {
                "/bin/mount",
                "-t",
                this->fstype,
                this->devnode,
                this->root,
            };

            logf_debug("Spawning: %s", mount);
            core::platform::process->invoke_check(mount);
        }
    }

    void RemovableContext::on_close()
    {
        core::platform::ArgVector argv = {
            "/bin/umount",
            this->devnode,
        };

        logf_debug("Spawning: %s", argv);
        core::platform::process->invoke_check(argv);
        Super::on_close();
    }
}  // namespace platform::vfs::local
