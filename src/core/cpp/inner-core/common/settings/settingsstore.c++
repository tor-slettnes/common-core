/// -*- c++ -*-
//==============================================================================
/// @file settingsstore.c++
/// @brief Read/write configuration from/to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "settings/settingsstore.h++"
//#include "string/misc.h++"
#include "parsers/json/reader.h++"
#include "parsers/json/writer.h++"
#include "parsers/yaml/reader.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include "platform/process.h++"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace core
{
    constexpr auto JSON_SUFFIX = ".json";
    constexpr auto YAML_SUFFIX = ".yaml";

    SettingsStore::SettingsStore(const types::PathList &filenames,
                                 const types::PathList &directories)
        : composite_(false),
          directories_(directories)
    {
        for (const auto &filename : filenames)
        {
            this->load(filename);
        }
    }

    SettingsStore::SettingsStore(const fs::path &filename,
                                 const types::PathList &directories)
        : composite_(false),
          directories_(directories)
    {
        this->load(filename);
    }

    bool SettingsStore::loaded() const
    {
        return !this->filenames_.empty();
    }

    bool SettingsStore::reload()
    {
        this->clear();
        return this->load(this->filenames_, false);
    }

    bool SettingsStore::load(const fs::path &filename,
                             bool update_filenames)
    {
        bool success = false;
        if (update_filenames)
        {
            this->filenames_.push_back(filename);
        }

        if (filename.is_absolute())
        {
            success = this->load_from(filename);
        }
        else
        {
            std::vector<fs::path> extended_names;
            if (!filename.extension().empty())
            {
                extended_names = {filename};
            }
            else
            {
                extended_names = {
                    platform::path->extended_filename(filename, JSON_SUFFIX),
                    platform::path->extended_filename(filename, YAML_SUFFIX),
                };
            }

            bool secondary = false;
            // Iterate through directory list,
            // giving preference to values from earlier file occurences
            for (const fs::path &folder : this->directories_)
            {
                for (const fs::path &extendedname : extended_names)
                {
                    if (this->load_from(folder / extendedname))
                    {
                        this->composite_ |= secondary;
                        success = true;
                        secondary = true;
                    }
                }
            }
        }

        return success;
    }

    bool SettingsStore::load(const std::vector<fs::path> &filenames,
                             bool update_filenames)
    {
        bool success = false;
        for (const fs::path &filename : filenames)
        {
            success |= this->load(filename, update_filenames);
        }
        return success;
    }

    bool SettingsStore::load_from(const fs::path &abspath)
    {
        types::Value value;

        try
        {
            if (abspath.extension() == JSON_SUFFIX)
            {
                value = json::reader.read_file(abspath);
            }
            else if (abspath.extension() == YAML_SUFFIX)
            {
                value = yaml::reader.read_file(abspath);
            }
            else
            {
                return false;
            }
        }
        catch (const fs::filesystem_error &)
        {
            return false;
        }
        catch (const std::system_error &)
        {
            return false;
        }
        catch (const std::exception &e)
        {
            str::format(std::cerr,
                        "\nFailed to load settings from %s: %s\n",
                        abspath,
                        e);
            return false;
        }

        if (auto kvmap = value.get_kvmap_ptr())
        {
            this->recursive_merge(*kvmap);
            return true;
        }
        else
        {
            return false;
        }
    }

    void SettingsStore::save(bool delta_only,
                             bool use_temp_file)
    {
        if (this->loaded())
        {
            this->save_to(this->filename(), delta_only, use_temp_file);
        }
    }

    void SettingsStore::save_to(const fs::path &filename,
                                bool delta_only,
                                bool use_temp_file) const
    {
        fs::path path = platform::path->config_folder() /
                        platform::path->extended_filename(filename, JSON_SUFFIX);

        // NOTE: there is no guarantee that `path` is located within `configFolder()`,
        // as `filename` may have been absolute.

        fs::create_directories(path.parent_path());

        if (use_temp_file)
        {
            fs::path temp_path = path;
            temp_path += ".tmp." + std::to_string(platform::process->process_id());

            this->write_to(temp_path, delta_only);
            fs::rename(temp_path, path);
        }
        else
        {
            this->write_to(path, delta_only);
        }
    }

    void SettingsStore::write_to(const fs::path &path,
                                 bool delta_only) const
    {
        if (delta_only)
        {
            json::writer.write_file(
                path,                                             // path
                this->recursive_delta(this->default_settings()),  // value
                true);                                            // pretty
        }
        else
        {
            json::writer.write_file(path, *this, true);
        }
    }

    SettingsStore SettingsStore::default_settings() const
    {
        return SettingsStore(
            this->filenames(),
            platform::path->default_settings_paths());
    }

    fs::path SettingsStore::filename() const
    {
        if (this->filenames_.size())
        {
            return this->filenames_.front();
        }
        else
        {
            return {};
        }
    }

    types::PathList SettingsStore::filenames() const
    {
        return this->filenames_;
    }

    types::PathList SettingsStore::directories() const
    {
        return this->directories_;
    }

    types::Value SettingsStore::extract_value(const types::ValueList &path,
                                              const types::Value &fallback) const noexcept
    {
        types::Value value = KeyValueMap::create_shared(*this);
        for (const types::Value &element : path)
        {
            if (element.is_string() && value.is_kvmap())
            {
                value = value.get(element.as_string());
            }
            else if (element.is_integral() && value.is_valuelist())
            {
                value = value.get(element.as_sint());
            }
            else
            {
                value = fallback;
            }
        }
        return value;
    }

    std::pair<SettingsStore::iterator, bool>
    SettingsStore::insert_value(const std::vector<std::string> &path,
                                const types::Value &value,
                                bool save)
    {
        types::KeyValueMap *current = this;
        std::pair<SettingsStore::iterator, bool> result = {{}, false};
        if (path.size())
        {
            for (auto path_it = path.begin(); path_it != path.end() - 1; path_it++)
            {
                types::Value &next_value = (*current)[*path_it];
                if (next_value.type() != types::ValueType::KVMAP)
                {
                    next_value = types::KeyValueMap::create_shared();
                }
                if (auto *next_map = std::get_if<types::KeyValueMapPtr>(&next_value))
                {
                    current = next_map->get();
                }
            }
            result = current->insert_or_assign(path.back(), value);
        }

        if (save)
        {
            this->save(true);
        }

        return result;
    }

}  // namespace core
