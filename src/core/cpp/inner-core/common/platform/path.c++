/// -*- c++ -*-
//==============================================================================
/// @file path.c++
/// @brief Path-related functions - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "path.h++"
#include "buildinfo.h"
#include <fstream>

#define LOCAL_FOLDER          "/shared"
#define DEFAULT_CONFIG_FOLDER LOCAL_FOLDER "/config"
#define DEFAULT_DATA_FOLDER   LOCAL_FOLDER "/data"
#define DEFAULT_LOG_FOLDER    LOCAL_FOLDER "/logs"
#define CONFIGPATH_VAR        "CONFIGPATH"
#define DATADIR_VAR           "DATADIR"
#define LOGDIR_VAR            "LOGDIR"

/// Default filesystem paths.
namespace core::platform
{
    PathProvider::PathProvider(const std::string &provider_name,
                               const std::string &exec_name)
        : Super(provider_name),
          exec_name_(exec_name)
    {
    }

    fs::path PathProvider::default_config_folder() const noexcept
    {
        return DEFAULT_CONFIG_FOLDER;
    }

    fs::path PathProvider::default_data_folder() const noexcept
    {
        return DEFAULT_DATA_FOLDER;
    }

    fs::path PathProvider::default_log_folder() const noexcept
    {
        return DEFAULT_LOG_FOLDER;
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
        const char *configpath = std::getenv(CONFIGPATH_VAR);
        if (configpath && *configpath)
        {
            types::PathList directorylist;
            for (fs::path path : str::split(configpath, this->path_separator()))
            {
                directorylist.push_back(fs::weakly_canonical(install_folder() / path));
            }
            return directorylist;
        }
        else
        {
            return {
                this->default_config_folder(),
                this->install_folder() / SETTINGS_DIR,
            };
        }
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

    /// Global instance, populated with the "best" provider for this system.
    ProviderProxy<PathProvider> path("path");

};  // namespace core::platform

namespace std::filesystem
{
    static std::unordered_map<file_type, std::string> typenames = {
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
        try
        {
            stream << typenames.at(type);
        }
        catch (const std::out_of_range &e)
        {
            stream << typenames.at(file_type::unknown);
        }
        return stream;
    }
}  // namespace std::filesystem
