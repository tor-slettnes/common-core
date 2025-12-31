/// -*- c++ -*-
//==============================================================================
/// @file path.c++
/// @brief Path-related functions - abstract provider
/// @author Tor Slettnes
//==============================================================================

#include "path.h++"
#include "buildinfo.h++"
#include <fstream>

#define CONFIGPATH_VAR "CONFIGPATH"
#define DATADIR_VAR    "DATADIR"
#define LOGDIR_VAR     "LOGDIR"

/// Default filesystem paths.
namespace core::platform
{
    //--------------------------------------------------------------------------
    // FileStats

    core::types::TaggedValueList &operator<<(
        core::types::TaggedValueList &tvlist,
        const FileStats &filestats)
    {
        tvlist.append("type", core::str::convert_from(filestats.type));
        tvlist.append("size", filestats.size);
        tvlist.append_if(!filestats.link.empty(), "link", filestats.link);
        tvlist.append("mode", core::str::format("0%03o", filestats.mode));
        tvlist.append("readable", filestats.readable);
        tvlist.append("writable", filestats.writable);
        tvlist.append("uid", filestats.uid);
        tvlist.append("gid", filestats.gid);
        tvlist.append("owner", filestats.owner);
        tvlist.append("group", filestats.group);
        tvlist.append("access_time", filestats.access_time);
        tvlist.append("modify_time", filestats.modify_time);
        tvlist.append("create_time", filestats.create_time);
        tvlist.append("attributes", filestats.attributes);
        return tvlist;
    }

    std::ostream &operator<<(
        std::ostream &stream,
        const FileStats &filestats)
    {
        return stream << core::types::TaggedValueList::create_from(filestats);
    }

    //--------------------------------------------------------------------------
    // PathProvider

    PathProvider::PathProvider(const std::string &provider_name,
                               const std::string &exec_name)
        : Super(provider_name),
          exec_name_(exec_name)
    {
    }

    std::optional<FileStats> PathProvider::try_get_stats(const fs::path &path,
                                                         bool dereference) const
    {
        try
        {
            return this->get_stats(path, dereference);
        }
        catch (const fs::filesystem_error &e)
        {
            return {};
        }
    }

    FileStats PathProvider::get_stats(const fs::path &path,
                                      bool dereference) const
    {
        fs::file_status status = dereference ? fs::symlink_status(path) : fs::status(path);
        std::size_t size = fs::is_regular_file(status) ? fs::file_size(path) : 0;

        // core::dt::TimePoint last_write_time =
        //     std::chrono::file_clock::to_sys(fs::last_write_time(localpath));

        return {
            .type = status.type(),
            .size = size,
            .link = this->readlink(path),
            .mode = static_cast<FileMode>(status.permissions()),
            .readable = ::access(path.c_str(), R_OK) == 0,
            .writable = ::access(path.c_str(), W_OK) == 0,
            // .uid = 0,
            // .gid = 0,
            // .owner = "",
            // .group = "",
            // .access_time = last_write_time,
            // .modfiy_time = {},
            // .create_time = {},
            // .attributes = {}
        };
    }

    std::optional<fs::path> PathProvider::user_config_folder() const noexcept
    {
        return {};
    }

    fs::path PathProvider::default_config_folder() const noexcept
    {
        return LOCAL_SETTINGS_DIR;
    }

    fs::path PathProvider::default_data_folder() const noexcept
    {
        return LOCAL_DATA_DIR;
    }

    fs::path PathProvider::default_log_folder() const noexcept
    {
        return LOGS_DIR;
    }

    fs::path PathProvider::exec_path() const noexcept
    {
        return this->exec_name_;
    }

    fs::path PathProvider::exec_folder_path() const noexcept
    {
        return this->exec_path().parent_path();
    }

    std::string PathProvider::exec_name(bool remove_extension,
                                        const std::string &fallback) const noexcept
    {
        fs::path exec_name = this->exec_path().filename();

        if (!exec_name.empty())
        {
            if (remove_extension)
            {
                exec_name = exec_name.replace_extension();
            }
            return exec_name.string();
        }
        else
        {
            return fallback;
        }
    }

    fs::path PathProvider::locate_dominating_folder(const fs::path &start,
                                                    const fs::path &name,
                                                    const fs::path &fallback) const noexcept
    {
        fs::path current = fs::weakly_canonical(start);
        while (!fs::exists(current / name))
        {
            if (!current.has_relative_path())
                return fallback;
            current = current.parent_path();
        }

        return current;
    }

    fs::path PathProvider::extended_filename(fs::path pathname,
                                             fs::path defaultSuffix,
                                             bool acceptAnySuffix) const noexcept
    {
        if (!pathname.filename().empty() &&
            (pathname.extension().empty() ||
             (!acceptAnySuffix && (pathname.extension() != defaultSuffix))))
        {
            pathname += defaultSuffix;
        }
        return pathname;
    }

    fs::path PathProvider::install_folder() const noexcept
    {
        return this->locate_dominating_folder(
            this->exec_folder_path(),  // start
            SETTINGS_DIR,              // name
            ".");                      // fallback
    }

    types::PathList PathProvider::settings_paths() const noexcept
    {
        types::PathList directorylist;

        const char *configpath = std::getenv(CONFIGPATH_VAR);
        if (configpath && *configpath)
        {
            for (fs::path path : str::split(configpath, this->path_separator()))
            {
                directorylist.push_back(fs::weakly_canonical(install_folder() / path));
            }
        }
        else
        {
            if (auto user_config_folder = this->user_config_folder())
            {
                directorylist.push_back(user_config_folder.value());
            }

            directorylist.push_back(this->default_config_folder());

            types::PathList default_paths = this->default_settings_paths();
            directorylist.insert(
                directorylist.end(),
                default_paths.begin(),
                default_paths.end());
        }
        return directorylist;
    }

    types::PathList PathProvider::default_settings_paths() const noexcept
    {
        return {
            this->install_folder() / SETTINGS_DIR,
        };
    }

    fs::path PathProvider::config_folder() const noexcept
    {
        return this->settings_paths().front();
    }

    fs::path PathProvider::data_folder() const noexcept
    {
        if (const char *datafolder = std::getenv(DATADIR_VAR))
        {
            return datafolder;
        }
        else
        {
            return this->default_data_folder();
        }
    }

    fs::path PathProvider::log_folder() const noexcept
    {
        if (const char *logfolder = std::getenv(LOGDIR_VAR))
        {
            return logfolder;
        }
        else
        {
            return this->default_log_folder();
        }
    }

    fs::path PathProvider::readlink(const fs::path &path) const noexcept
    {
        return {};
    }

    types::ByteVector PathProvider::readdata(const fs::path &path,
                                             ssize_t maxsize) const noexcept
    {
        types::ByteVector buffer;
        fs::file_status status = fs::status(path);
        if (fs::is_regular_file(status))
        {
            std::streampos bufsize = fs::file_size(path);
            if (bufsize > maxsize && maxsize > 0)
            {
                bufsize = maxsize;
            }

            std::ifstream file(path, std::ios::binary);
            buffer.resize(bufsize);
            file.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        }
        return buffer;
    }

    std::string PathProvider::readtext(const fs::path &path,
                                       const std::set<char> &striptrailing,
                                       ssize_t maxsize,
                                       ssize_t bufsize) const noexcept
    {
        std::string text;
        fs::file_status status = fs::status(path);
        if (fs::is_regular_file(status))
        {
            if (bufsize == 0)
            {
                bufsize = fs::file_size(path);
            }

            if (bufsize > maxsize && maxsize > 0)
            {
                bufsize = maxsize;
            }

            std::ifstream file(path);
            text.resize(bufsize);
            file.read(text.data(), text.size());

            while (text.size() && striptrailing.count(text.back()))
            {
                text.pop_back();
            }
        }
        return text;
    }

    fs::path PathProvider::mktempdir(const std::string &prefix,
                                     const std::string &suffix)
    {
        return this->mktempdir(this->tempfolder(), prefix, suffix);
    }

    fs::path PathProvider::mktemp(const std::string &prefix,
                                  const std::string &suffix)
    {
        return this->mktemp(this->tempfolder(), prefix, suffix);
    }

    bool PathProvider::filename_match(
        const types::PathList &masks,
        const fs::path &path,
        bool match_leading_period,
        bool ignore_case) const
    {
        for (const std::string &mask : masks)
        {
            if (this->filename_match(mask, path, match_leading_period, ignore_case))
            {
                return true;
            }
        }
        return false;
    }

    std::vector<fs::directory_entry> PathProvider::glob(
        const types::PathList &filename_masks,
        const fs::path &directory,
        bool match_leading_period,
        bool ignore_case,
        bool recursive) const
    {
        std::vector<fs::directory_entry> hits;
        this->locate_inside(directory,
                            filename_masks,
                            match_leading_period,
                            ignore_case,
                            recursive,
                            &hits);
        return hits;
    }

    std::vector<fs::directory_entry> PathProvider::locate(
        const types::PathList &filename_masks,
        const fs::path &directory,
        bool match_leading_period,
        bool ignore_case) const
    {
        return this->glob(filename_masks,
                          directory,
                          match_leading_period,
                          ignore_case,
                          true);
    }

    void PathProvider::locate_inside(
        const fs::path &directory,
        const types::PathList &filename_masks,
        bool match_leading_period,
        bool ignore_case,
        bool recursive,
        std::vector<fs::directory_entry> *dir) const
    {
        for (auto &pi : fs::directory_iterator(directory))
        {
            if (this->filename_match(filename_masks,
                                     pi.path(),
                                     match_leading_period,
                                     ignore_case))
            {
                dir->push_back(pi);
            }

            if (recursive && (pi.status().type() == fs::file_type::directory))
            {
                this->locate_inside(pi.path(),
                                    filename_masks,
                                    match_leading_period,
                                    ignore_case,
                                    true,
                                    dir);
            }
        }
    }

    /// Global instance, populated with the "best" provider for this system.
    ProviderProxy<PathProvider> path("path");

};  // namespace core::platform

namespace std::filesystem
{
    static core::types::SymbolMap<file_type> typenames = {
        {file_type::none, "none"},
        {file_type::not_found, "not_found"},
        {file_type::regular, "regular"},
        {file_type::directory, "directory"},
        {file_type::symlink, "symlink"},
        {file_type::block, "block"},
        {file_type::character, "character"},
        {file_type::fifo, "fifo"},
        {file_type::socket, "socket"},
        {file_type::unknown, "unknown"},
    };

    std::ostream &operator<<(std::ostream &stream, const file_type &type)
    {
        return typenames.to_stream(stream, type, typenames.at(file_type::unknown));
    }
}  // namespace std::filesystem
