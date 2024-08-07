// -*- c++ -*-
//==============================================================================
/// @file vfs-local-provider.c++
/// @brief VFS service - native implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-local-provider.h++"
#include "vfs-attribute-store.h++"
#include "vfs.h++"
#include "status/exceptions.h++"
#include "platform/path.h++"

#include <chrono>

// #include <sys/types.h>
// #include <sys/stat.h>
// #include <sys/statvfs.h>  // statvfs(3)
// #include <unistd.h>       // stat(2)
// #include <pwd.h>          // getpwuid(3)
// #include <grp.h>          // getgruid(3)
// #include <fnmatch.h>      // fnmatch(3)

#include <cstring>  // std::memset()

namespace platform::vfs::local
{
    LocalProvider::LocalProvider(const std::string &name,
                                 core::platform::ProviderPriority priority)
        : Super(name, priority),
          settings(SETTINGS_STORE)
    {
    }

    void LocalProvider::initialize()
    {
        Provider::initialize();
        this->loadContexts();
    }

    ContextMap LocalProvider::getContexts() const
    {
        return this->contexts;
    }

    ContextMap LocalProvider::getOpenContexts() const
    {
        ContextMap cmap;
        for (const auto &[key, ref] : this->contexts)
        {
            if (auto cxt = std::dynamic_pointer_cast<LocalContext>(ref))
            {
                if (cxt->refcount > 0)
                {
                    cmap[key] = cxt;
                }
            }
        }
        return cmap;
    }

    ContextRef LocalProvider::getContext(const std::string &name, bool required) const
    {
        try
        {
            return this->contexts.at(name);
        }
        catch (const std::out_of_range &e)
        {
            if (required)
            {
                auto msg = core::str::format("Filesystem context %r does not exist", name);
                throw core::exception::NotFound(msg, name);
            }
            else
            {
                return {};
            }
        }
    }

    ContextRef LocalProvider::openContext(const std::string &name, bool required)
    {
        if (ContextRef cxt = this->getContext(name, required))
        {
            cxt->add_ref();
            return cxt;
        }
        else
        {
            return {};
        }
    }

    void LocalProvider::closeContext(const ContextRef &cxt)
    {
        if (cxt)
        {
            cxt->del_ref();
        }
    }

    VolumeStats LocalProvider::volumeStats(const Path &vpath,
                                           const OperationFlags &flags) const
    {
        Location loc = vfs::location(vpath, false);
        fs::path lpath = loc.localPath();
        return fs::space(lpath);
    }

    FileStats LocalProvider::fileStats(const Path &vpath,
                                       const OperationFlags &flags) const
    {
        Location loc = vfs::location(vpath, false);
        fs::path lpath = loc.localPath();
        FileStats stats = this->read_stats(lpath, flags.dereference);
        if (flags.with_attributes)
        {
            stats.attributes = AttributeStore(lpath, stats.type).get_attributes();
        }
        return stats;
    }

    Directory LocalProvider::getDirectory(const Path &vpath,
                                          const OperationFlags &flags) const
    {
        Location loc = vfs::location(vpath, false);
        fs::path lpath = loc.localPath();

        Directory dir;
        for (auto &pi : fs::directory_iterator(lpath))
        {
            std::string basename = pi.path().filename();
            if (flags.include_hidden || (basename.substr(0, 1) != "."))
            {
                dir[basename] = this->read_stats(lpath / basename,
                                                 flags.dereference);
            }
        }

        if (flags.with_attributes)
        {
            AttributeStore store(lpath, fs::file_type::directory);
            if (!store.empty())
            {
                for (auto &entry : dir)
                {
                    entry.second.attributes = store.get(entry.first.string()).as_kvmap();
                }
            }
        }

        return dir;
    }

    Directory LocalProvider::locate(const Path &root,
                                    const std::vector<std::string> &filename_masks,
                                    const core::types::TaggedValueList &attributes,
                                    const OperationFlags &flags) const
    {
        Location loc = vfs::location(root, false);
        Directory dir;
        LocalProvider::locate_inside(loc.localPath(),
                                     "",
                                     filename_masks,
                                     attributes,
                                     flags.with_attributes,
                                     flags.include_hidden,
                                     flags.ignore_case,
                                     &dir);
        return dir;
    }

    void LocalProvider::copy(const Paths &sources,
                             const Path &target,
                             const OperationFlags &flags) const
    {
        if ((sources.size() > 1) && !flags.inside_target)
        {
            throw core::exception::InvalidArgument(
                "Multiple source paths require 'inside_target' option");
        }

        LocationList srclocs = vfs::locations(sources, false);
        Location tgtloc = vfs::location(target, true);

        for (const Location &srcloc : srclocs)
        {
            copy2(srcloc, tgtloc, flags);
        }
    }

    void LocalProvider::move(const Paths &sources,
                             const Path &target,
                             const OperationFlags &flags) const
    {
        if ((sources.size() > 1) && !flags.inside_target)
        {
            throw core::exception::InvalidArgument(
                "Multiple source paths require 'inside_target' option");
        }

        LocationList srclocs = vfs::locations(sources, false);
        Location tgtloc = vfs::location(target, true);

        for (const Location &srcloc : srclocs)
        {
            move2(srcloc, tgtloc, flags);
        }
    }

    void LocalProvider::createFolder(const Path &vpath,
                                     const OperationFlags &flags) const
    {
        Location loc = vfs::location(vpath, true);
        if (flags.force)
        {
            fs::create_directories(loc.localPath());
        }
        else
        {
            fs::create_directory(loc.localPath());
        }
    }

    void LocalProvider::remove(const Paths &sources,
                               const OperationFlags &flags) const
    {
        for (const Location &loc : vfs::locations(sources, true))
        {
            LocalProvider::remove(loc, flags);
        }
    }

    ReaderRef LocalProvider::readFile(const Path &vpath) const
    {
        return std::make_unique<FileReader>(vfs::location(vpath, false));
    }

    WriterRef LocalProvider::writeFile(const Path &vpath) const
    {
        return std::make_unique<FileWriter>(vfs::location(vpath, true));
    }

    void LocalProvider::addContext(const std::string &name, ContextRef cxt)
    {
        log_message(cxt->removable
                        ? core::status::Level::DEBUG
                        : core::status::Level::TRACE,
                    "Addding VFS context: ",
                    *cxt);

        this->contexts[name] = cxt;
        vfs::signal_context.emit(core::signal::MAP_ADDITION, name, cxt);
    }

    bool LocalProvider::removeContext(const std::string &name)
    {
        auto nh = this->contexts.extract(name);
        if (nh)
        {
            logf_message(nh.mapped()->removable
                             ? core::status::Level::DEBUG
                             : core::status::Level::TRACE,
                         "Removing VFS context: %s",
                         *nh.mapped());

            if (auto cxt = std::dynamic_pointer_cast<LocalContext>(nh.mapped()))
            {
                if (cxt->refcount > 0)
                {
                    cxt->close();
                }
            }

            vfs::signal_context.clear(name);
        }
        return bool(nh);
    }

    void LocalProvider::loadContexts(void)
    {
        if (core::types::KeyValueMapRef cxts = this->settings.get(SETTING_CONTEXTS).get_kvmap())
        {
            for (const auto &[key, value] : *cxts)
            {
                this->addContext(key, this->newContext(key, value));
            }
        }
    }

    ContextRef LocalProvider::newContext(
        const std::string &name,
        const core::types::Value &settings)
    {
        return std::make_shared<LocalContext>(
            name,
            settings.get(SETTING_CXT_ROOT).as_string(),
            settings.get(SETTING_CXT_WRITABLE, true).as_bool(),
            settings.get(SETTING_CXT_PREEXEC).as_string(),
            settings.get(SETTING_CXT_POSTEXEC).as_string(),
            settings.get(SETTING_CXT_TITLE).as_string());
    }

    core::types::KeyValueMap LocalProvider::getAttributes(const Path &vpath) const
    {
        Location loc = vfs::location(vpath, false);
        return this->getAttributes(loc.localPath());
    }

    void LocalProvider::setAttributes(
        const Path &vpath,
        const core::types::KeyValueMap &attributes) const
    {
        Location loc = vfs::location(vpath, true);
        this->setAttributes(loc.localPath(), attributes);
    }

    void LocalProvider::clearAttributes(const Path &vpath) const
    {
        Location loc = vfs::location(vpath, true);
        this->clearAttributes(loc.localPath());
    }

    //==========================================================================
    // Static methods

    void LocalProvider::copy2(const Location &srcloc,
                              const Location &tgtloc,
                              const OperationFlags &flags) const
    {
        fs::path localsource = srcloc.localPath();
        fs::path localtarget = tgtloc.localPath();

        if (flags.inside_target)
        {
            localtarget /= localsource.filename();
        }

        fs::copy_options options = fs::copy_options::recursive;
        if (flags.update)
            options |= fs::copy_options::update_existing;
        else
            options |= fs::copy_options::overwrite_existing;

        if (!flags.dereference)
            options |= fs::copy_options::copy_symlinks;

        if (flags.force)
        {
            if (fs::exists(localtarget) &&
                (!flags.merge ||
                 !fs::is_directory(localtarget) ||
                 !fs::is_directory(localsource)))
            {
                fs::remove_all(localtarget);
            }
            else if (!fs::is_directory(localtarget.parent_path()))
            {
                fs::create_directories(localtarget.parent_path());
            }
        }
        else if (fs::is_directory(localtarget) &&
                 (!flags.merge || !fs::is_directory(localsource)))
        {
            throw core::exception::FilesystemError(EISDIR, localtarget);
        }

        try
        {
            fs::copy(localsource, localtarget, options);
        }
        catch (const fs::filesystem_error &e)
        {
            logf_info("Copy error; localsource %r exists? %b",
                      localsource,
                      fs::exists(localsource));
            throw;
        }
        catch (const std::exception &e)
        {
            logf_info("Copy unknown error: %s", e);
            throw;
        }

        if (flags.with_attributes)
        {
            AttributeStore srcstore(localsource);
            core::types::KeyValueMap attributes = srcstore.get_attributes();

            AttributeStore tgtstore(localtarget);
            tgtstore.set_attributes(attributes);
        }
    }

    void LocalProvider::move2(const Location &srcloc,
                              const Location &tgtloc,
                              const OperationFlags &flags) const
    {
        fs::path localsource = srcloc.localPath();
        fs::path localtarget = tgtloc.localPath();

        if (flags.inside_target)
        {
            localtarget /= localsource.filename();
        }

        if (flags.force)
        {
            if (fs::exists(localtarget))
            {
                fs::remove_all(localtarget);
            }
            else if (!fs::is_directory(localtarget.parent_path()))
            {
                fs::create_directories(localtarget.parent_path());
            }
        }

        fs::rename(localsource, localtarget);

        if (flags.with_attributes)
        {
            AttributeStore srcstore(localsource);
            core::types::KeyValueMap attributes = srcstore.get_attributes();
            srcstore.clear({}, true);

            AttributeStore tgtstore(localtarget);
            tgtstore.set_attributes(attributes, true);
        }
    }

    void LocalProvider::remove(const Location &loc,
                               const OperationFlags &flags) const
    {
        fs::path lpath = loc.localPath();
        if (flags.force)
        {
            fs::remove_all(lpath);
        }
        else
        {
            fs::remove(lpath);
        }

        if (flags.with_attributes)
        {
            AttributeStore store(lpath);
            store.clear();
        }
    }

    FileStats LocalProvider::read_stats(
        const fs::path &localpath,
        bool dereference) const
    {
        fs::file_status status =
            dereference ? fs::symlink_status(localpath)
                        : fs::status(localpath);

        std::size_t size =
            fs::is_regular_file(status) ? fs::file_size(localpath)
                                        : 0;

        // core::dt::TimePoint last_write_time =
        //     fs::to_sys(fs::last_write_time(localpath));

        return {
            .type = status.type(),
            .size = size,
            .link = core::platform::path->readlink(localpath),
            .mode = 0,
            .readable = access(localpath.c_str(), R_OK) == 0,
            .writable = access(localpath.c_str(), W_OK) == 0,
            // .uid = 0,
            // .gid = 0,
            // .owner = "",
            // .group = "",
            // .accessTime = {},
            // .modfiyTime = {},
            // .createTime = {},
            // .attributes = {}
        };
    }

    void LocalProvider::locate_inside(
        const fs::path &root,
        const fs::path &relpath,
        const std::vector<std::string> &filename_masks,
        const core::types::TaggedValueList &attribute_filters,
        bool with_attributes,
        bool include_hidden,
        bool ignore_case,
        Directory *dir) const
    {
        for (auto &pi : fs::directory_iterator(root / relpath))
        {
            fs::path basename = pi.path().filename();
            if (pi.status().type() == fs::file_type::directory)
            {
                this->locate_inside(root,
                                    relpath / basename,
                                    filename_masks,
                                    attribute_filters,
                                    with_attributes,
                                    include_hidden,
                                    ignore_case,
                                    dir);
            }
            else if (this->filename_match(filename_masks, basename, include_hidden, ignore_case))
            {
                core::types::KeyValueMap attributes;
                if (with_attributes || attribute_filters.size())
                {
                    attributes = this->getAttributes(pi.path(), pi.status().type());
                }

                if (attribute_filters.empty() ||
                    attribute_match(attribute_filters, attributes))
                {
                    FileStats stats = read_stats(pi.path());
                    stats.attributes = std::move(attributes);
                    dir->insert_or_assign(relpath / basename, std::move(stats));
                }
            }
        }
    }

    bool LocalProvider::filename_match(
        const std::vector<std::string> &masks,
        const fs::path &basename,
        bool include_hidden,
        bool ignore_case) const
    {
        for (const std::string &mask : masks)
        {
            if (mask == basename)
            {
                return true;
            }
        }
        return false;
    }

    bool LocalProvider::attribute_match(
        const core::types::TaggedValueList &attribute_filters,
        const core::types::KeyValueMap &attributes) const
    {
        for (const auto &[name, value] : attribute_filters)
        {
            if (!name || (attributes.get(name.value()) != value))
            {
                return false;
            }
        }
        return true;
    }

    core::types::KeyValueMap LocalProvider::getAttributes(const fs::path &localpath,
                                                          fs::file_type type_hint) const
    {
        return AttributeStore(localpath, type_hint).get_attributes();
    }

    void LocalProvider::setAttributes(const fs::path &localpath,
                                      const core::types::KeyValueMap &attributes,
                                      fs::file_type type_hint,
                                      bool save) const
    {
        AttributeStore(localpath, type_hint).set_attributes(attributes, save);
    }

    void LocalProvider::clearAttributes(const fs::path &localpath,
                                        fs::file_type type_hint,
                                        bool save) const
    {
        AttributeStore(localpath, type_hint).clear();
    }
}  // namespace platform::vfs::local
