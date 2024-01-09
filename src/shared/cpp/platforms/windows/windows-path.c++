/// -*- c++ -*-
//==============================================================================
/// @file windows-path.c++
/// @brief Path-related functions - Windows specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "windows-path.h++"

#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
typedef unsigned int pid_t;
#endif

namespace cc::platform
{
    WindowsPathProvider::WindowsPathProvider(const std::string &programpath)
        : Super("WindowsPathProvider", programpath)
    {
    }

    uint WindowsPathProvider::path_max_size() const noexcept
    {
        return 1024;
    }

    std::string WindowsPathProvider::path_separator() const noexcept
    {
        return ";";
    }

    std::string WindowsPathProvider::dir_separator() const noexcept
    {
        return "\\";
    }

    fs::path WindowsPathProvider::devnull() const noexcept
    {
        return "NUL:";
    }

    fs::path WindowsPathProvider::tempfolder() const noexcept
    {
        return "C:\\TEMP";
    }

    fs::path WindowsPathProvider::default_config_folder() const noexcept
    {
        return (this->fs_root() / Super::default_config_folder());
    }

    fs::path WindowsPathProvider::default_data_folder() const noexcept
    {
        return (this->fs_root() / Super::default_data_folder());
    }

    fs::path WindowsPathProvider::default_log_folder() const noexcept
    {
        return (this->fs_root() / Super::default_log_folder());
    }

    fs::path WindowsPathProvider::exec_path() const noexcept
    {
        wchar_t path[MAX_PATH] = {0};
        GetModuleFileNameW(NULL, path, MAX_PATH);
        return path;
    }

    fs::path WindowsPathProvider::readlink(const fs::path &path) const noexcept
    {
        return path;
    }

    fs::path WindowsPathProvider::mktemp(const fs::path &folder,
                                         const std::string &prefix,
                                         const std::string &suffix)
    {
        fs::path path = folder;
        path /= prefix + "XXXXXX";
        std::string name = path.string();
        if (int err = _mktemp_s(name.data(), name.size()))
        {
            throw fs::filesystem_error(
                strerror(err),
                path,
                std::error_code(err, std::system_category()));
        }
        path += suffix;
        return path;
    }

    fs::path WindowsPathProvider::fs_root() const
    {
        return "C:";
    }

}  // namespace cc::platform
