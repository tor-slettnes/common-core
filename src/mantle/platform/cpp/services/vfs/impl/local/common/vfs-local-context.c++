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
    //==========================================================================
    // LocalContext

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


    //==========================================================================
    // LocalLocation

    // VolumeStats LocalLocation::volume_stats(
    //     const fs::path &relpath,
    //     const OperationFlags &flags) const
    // {
    //     return fs::space(this->localPath(relpath));
    // }

    // FileStats LocalLocation::file_stats(
    //     const fs::path &relpath,
    //         bool dereference = false,
    //         bool with_attributes = false) const
    // {
    //     fs::path localpath = this->localPath(relpath);
    //     FileStats stats = this->read_stats(localpath, dereference);
    //     if (with_attributes)
    //     {
    //         stats.attributes = AttributeStore(localpath);
    //     }
    //     return stats;
    // }

    // Directory LocalLocation::get_directory(
    //     const fs::path &relpath,
    //     const OperationFlags &flags) const
    // {
    //     fs::path lpath = this->localPath(relpath);

    //     Directory dir;
    //     for (auto &pi : fs::directory_iterator(lpath))
    //     {
    //         std::string basename = pi.path().filename();
    //         if (flags.include_hidden || (basename.substr(0, 1) != "."))
    //         {
    //             dir[basename] = this->read_stats(lpath / basename,
    //                                              flags.dereference);
    //         }
    //     }

    //     if (flags.with_attributes)
    //     {
    //         AttributeStore store(lpath, fs::file_type::directory);
    //         if (!store.empty())
    //         {
    //             for (auto &entry : dir)
    //             {
    //                 entry.second.attributes = store.get(entry.first.string()).as_kvmap();
    //             }
    //         }
    //     }

    //     return dir;
    // }

    // Directory LocalLocation::locate(
    //     const fs::path &relpath,
    //     const std::vector<std::string> &filename_masks,
    //     const core::types::TaggedValueList &attribute_filters,
    //     const OperationFlags &flags) const
    // {
    //     Directory dir;
    //     This::locate_inside(loc.localPath(),
    //                                  "",
    //                                  filename_masks,
    //                                  attributes,
    //                                  flags.with_attributes,
    //                                  flags.include_hidden,
    //                                  flags.ignore_case,
    //                                  &dir);
    //     return dir;
    // }

    // void LocalLocation::copy_from(
    //     const LocationList &sources,
    //     const fs::path &relpath,
    //     bool inside_target = false) const
    // {
    //     if ((sources.size() > 1) && !inside_target)
    //     {
    //         throw core::exception::InvalidArgument(
    //             "Multiple source paths require 'inside_target' option");
    //     }

    //     this->check_modify_access();

    //     for (const Location &source : sources)
    //     {
    //         this->copy2(source, this-
    //         copy2(srcloc, tgtloc, flags);
    //     }
    // }

    // void LocalLocation::move_from(
    //     const ContextProxy &other_context,
    //     const fs::path &other_path,
    //     const fs::path &this_path,
    //     const OperationFlags &flags) const
    // {
    // }

    // void LocalLocation::remove(
    //     const fs::path &relpath,
    //     const OperationFlags &flags) const
    // {
    // }

    // void LocalLocation::create_folder(
    //     const fs::path &relpath,
    //     const OperationFlags &flags) const
    // {
    // }

    // UniqueReader LocalLocation::read_file(
    //     const fs::path &relpath) const
    // {
    // }

    // UniqueWriter LocalLocation::write_file(
    //     const fs::path &relpath) const
    // {
    // }

    // core::types::KeyValueMap LocalLocation::get_attributes(
    //     const fs::path &relpath) const
    // {
    // }

    // void LocalLocation::set_attributes(
    //     const fs::path &relpath,
    //     const core::types::KeyValueMap &attributes) const
    // {
    // }

    // void LocalLocation::clear_attributes(
    //     const fs::path &relpath) const
    // {
    // }

}  // namespace platform::vfs::local
