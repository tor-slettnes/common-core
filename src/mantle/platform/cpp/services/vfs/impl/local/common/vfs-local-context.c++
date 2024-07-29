// -*- c++ -*-
//==============================================================================
/// @file vfs-local-context.c++
/// @brief VFS service - local filesystem context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-local-context.h++"
#include "vfs-signals.h++"
#include "status/exceptions.h++"


namespace platform::vfs::local
{
    LocalContext::LocalContext(const std::string &name,
                               const fs::path &root,
                               bool writable,
                               const std::string &preexec,
                               const std::string &postexec,
                               const std::string &title,
                               bool removable)
        : Context(name, root, writable, removable, title),
          preexec(preexec),
          postexec(postexec),
          refcount(0)
    {
    }

    void LocalContext::add_ref()
    {
        std::scoped_lock lck(this->mtx);
        logf_trace("Incrementing context %s current refcount %d",
                   this->name,
                   this->refcount);
        if (this->refcount == 0)
        {
            this->open();
        }
        this->refcount++;
    }

    void LocalContext::del_ref()
    {
        std::scoped_lock lck(this->mtx);
        logf_trace("Decrementing context %s current refcount %d",
                   this->name,
                   this->refcount);

        if ((this->refcount > 0) && (--refcount == 0))
        {
            this->close();
        }
    }

    void LocalContext::open()
    {
        logf_message(this->removable
                         ? core::status::Level::DEBUG
                         : core::status::Level::TRACE,
                     "Opening VFS context %r",
                     this->name);

        if (!fs::is_directory(this->root))
        {
            logf_debug("Creating context %r root folder %r", this->name, this->root);
            fs::create_directories(this->root);
        }

        this->on_open();
        signal_context_in_use.emit(core::signal::MAP_ADDITION,
                                   this->name,
                                   this->shared_from_this());
    }

    void LocalContext::close()
    {
        logf_message(this->removable
                         ? core::status::Level::DEBUG
                         : core::status::Level::TRACE,
                     "Closing VFS context %r",
                     this->name);

        try
        {
            this->on_close();
        }
        catch (...)
        {
            logf_notice("Failed to run close hooks for context %r: %s",
                        this->name,
                        std::current_exception());
        }

        if (fs::is_directory(this->root) && fs::is_empty(this->root))
        {
            logf_debug("Removing context %r root folder %r", this->name, this->root);
            try
            {
                fs::remove(this->root);
            }
            catch (const fs::filesystem_error &e)
            {
                logf_info("Could not remove context %r root folder %r: %s",
                          this->name,
                          this->root,
                          e);
            }
        }

        vfs::signal_context_in_use.emit(core::signal::MAP_REMOVAL,
                                        this->name,
                                        this->shared_from_this());
    }

    void LocalContext::on_open()
    {
        if (!this->preexec.empty() && system(this->preexec.c_str()) == -1)
        {
            throwf(core::exception::SystemError,
                   ("Could run context %r pre-exec hook", this->name));
        }
    }

    void LocalContext::on_close()
    {
        if (!this->postexec.empty() && system(this->postexec.c_str()) == -1)
        {
            throwf(core::exception::SystemError,
                   ("Could run context %r post-exec hook", this->name));
        }
    }
}  // namespace platform::vfs::local
