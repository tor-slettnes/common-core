// -*- c++ -*-
//==============================================================================
/// @file vfs-local-location.h++
/// @brief VFS service - local filesytem location handle
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-local-location.h++"

namespace platform::vfs::local
{
    //==========================================================================
    // LocalLocation

    VolumeStats LocalLocation::volume_stats(
        const OperationFlags &flags) const
    {
        return fs::space(this->localPath());
    }

    FileStats LocalLocation::file_stats(
        const OperationFlags &flags) const
    {
        fs::path localpath = this->localPath();
        FileStats stats = this->read_stats(localpath, dereference);
        if (flags.with_attributes)
        {
            stats.attributes = this->attribute_store()->get_attributes();
        }
        return stats;
    }

    Directory LocalLocation::list_folder(
        const OperationFlags &flags) const
    {
        fs::path lpath = this->localPath();

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
            auto store = this->attribute_store(fs::file_type::directory);
            if (!store->empty())
            {
                for (auto &entry : dir)
                {
                    entry.second.attributes = store->get(entry.first.string()).as_kvmap();
                }
            }
        }

        return dir;
    }

    Directory LocalLocation::locate(
        const std::vector<std::string> &filename_masks,
        const core::types::TaggedValueList &attribute_filters,
        const OperationFlags &flags) const
    {
        Directory dir;
        This::locate_inside(this->localPath(),
                            {},
                            filename_masks,
                            attributes,
                            flags.with_attributes,
                            flags.include_hidden,
                            flags.ignore_case,
                            &dir);
        return dir;
    }

    void LocalLocation::copy_from(
        const LocationList &sources,
        const OperationFlags &flags) const
    {
        if ((sources.size() > 1) && !flags.inside_target)
        {
            throw core::exception::InvalidArgument(
                "Multiple source paths require 'inside_target' option");
        }

        this->check_modify_access();
        fs::path localpath = this->localPath();
        for (const Location &source : sources)
        {
            this->copy_from(source, flags);
        }
    }

    void LocalLocation::move_from(
        const LocationList &sources,
        const OperationFlags &flags) const
    {
        if ((sources.size() > 1) && !flags.inside_target)
        {
            throw core::exception::InvalidArgument(
                "Multiple source paths require 'inside_target' option");
        }

        this->check_modify_access();
        for (const Location &source : sources)
        {
            source.check_modify_access();
        }

        fs::path localpath = this->localPath();
        for (const Location &source : sources)
        {
            this->move_from(source, flags);
        }
    }

    void LocalLocation::remove(
        const OperationFlags &flags) const
    {
        this->check_modify_access();
        fs::path localpath = this->localPath();

        if (flags.force)
        {
            fs::remove_all(localpath);
        }
        else
        {
            fs::remove(localpath);
        }

        if (flags.with_attributes)
        {
            this->attribute_store()->clear_attributes();
        }
    }

    void LocalLocation::create_folder(
        const OperationFlags &flags) const
    {
        this->check_modify_access();
        if (flags.force)
        {
            fs::create_directories(this->localPath());
        }
        else
        {
            fs::create_directory(this->localPath());
        }
    }

    UniqueReader LocalLocation::read() const
    {
        return std::make_unique<FileReader>(*this);
    }

    UniqueWriter LocalLocation::write() const
    {
        this->check_modify_access();
        return std::make_unique<FileWriter>(*this);
    }

    core::types::KeyValueMap LocalLocation::get_attributes() const
    {
        return this->attribute_store().get_attributes();
    }

    void LocalLocation::set_attributes(
        const core::types::KeyValueMap &attributes) const
    {
        this->check_modify_access();
        this->attribute_store().set_attributes(attributes);
    }

    void LocalLocation::clear_attributes() const
    {
        this->check_modify_access();
        this->attribute_store().clear_attributes();
    }

    //==========================================================================
    // Private methods

    FileStats LocalLocation::read_stats(
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
            .readable = ::access(localpath.c_str(), R_OK) == 0,
            .writable = ::access(localpath.c_str(), W_OK) == 0,
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

    void LocalLocation::copy_from(const Location &source,
                                  const OperationFlags &flags) const
    {
        fs::path localsource = source.localPath();
        fs::path localtarget = this->localPath();

        if (flags.inside_target)
        {
            localtarget /= localsource.filename();
        }

        fs::copy_options options = fs::copy_options::recursive;
        options |= (flags.update ? fs::copy_options::update_existing
                                 : fs::copy_options::overwrite_existing);

        if (!flags.dereference)
        {
            options |= fs::copy_options::copy_symlinks;
        }

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

        fs::copy(localsource, localtarget, options);

        if (flags.with_attributes)
        {
            core::types::KeyValueMap attributes = source.get_attributes();
            this->set_attributes(attributes);
        }
    }

    void LocalLocation::move_from(const Location &source,
                                  const OperationFlags &flags) const
    {
        fs::path localsource = source.localPath();
        fs::path localtarget = this->localPath();

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
            core::types::KeyValueMap attributes = source.get_attributes();
            this->set_attributes(attributes);
            source.clear_attributes();
        }
    }

    void LocalLocation::locate_inside(
        const fs::path &root,
        const fs::path &relpath,
        const std::vector<std::string> &filename_masks,
        const core::types::TaggedValueList &attribute_filters,
        bool with_attributes,
        bool include_hidden,
        bool ignore_case,
        Directory *dir) const
    {
        std::shared_ptr<AttributeStore> attribute_store;
        if (with_attributes || !attribute_filters.empty())
        {
            attribute_store = std::make_shared<AttributeStore>(
                root / relpath,
                fs::file_type::directory);
        }

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
            else if (this->filename_match(filename_masks,
                                          basename,
                                          include_hidden,
                                          ignore_case))
            {
                if (attribute_filters.empty() ||
                    this->attribute_match(attribute_filters, attributes))
                {
                    FileStats stats = this->read_stats(pi.path());
                    if (attribute_store)
                    {
                        stats.attributes = attribute_store->get_attributes(basename);
                    }
                    dir->insert_or_assign(relpath / basename, std::move(stats));
                }
            }
        }
    }

    bool LocalLocation::filename_match(
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

    bool LocalLocation::attribute_match(
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

    std::shared_ptr<AttributeStore> LocalLocation::attribute_store(
        fs::file_type type_hint) const
    {
        if (!this->attribute_store_)
        {
            this->attribute_store_ = std::make_shared<AttributeStore>(this->localPath(), type_hint);
        }
        return this->attribute_store_;
    }
}  // namespace platform::vfs::local
