/// -*- c++ -*-
//==============================================================================
/// @file settingsstore.c++
/// @brief Read/write configuration from/to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "config/settingsstore.h++"
//#include "string/misc.h++"
#include "json/jsonreader.h++"
#include "json/jsonwriter.h++"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace shared
{
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

    bool SettingsStore::load(const fs::path &filename)
    {
        bool success = false;
        fs::path extendedname = platform::path->extended_filename(filename, ".json");
        this->filenames_.push_back(extendedname);

        if (extendedname.is_absolute())
        {
            success = this->load_from(extendedname);
        }
        else
        {
            bool secondary_folder = false;
            // Iterate through directory list,
            // giving preference to values from earlier file occurences
            for (const fs::path &folder : this->directories_)
            {
                if (this->load_from(folder / extendedname))
                {
                    this->composite_ |= secondary_folder;
                    success = true;
                }
                secondary_folder = true;
            }
        }

        return success;
    }

    bool SettingsStore::load(const std::vector<fs::path> &filenames)
    {
        bool success = false;
        for (const fs::path &filename : filenames)
        {
            success |= this->load(filename);
        }
        return success;
    }

    bool SettingsStore::load_from(const fs::path &abspath)
    {
        types::Value value;

        try
        {
            value = json::JsonReader::read_from(abspath);
        }
        catch (const fs::filesystem_error &)
        {
            return false;
        }

        if (auto *kvmap = std::get_if<types::KeyValueMapRef>(&value))
        {
            this->recursive_merge(**kvmap);
            return true;
        }
        else
        {
            return false;
        }
    }

    void SettingsStore::save(bool delta_only)
    {
        this->save_to(this->filename(), delta_only);
    }

    void SettingsStore::save_to(const fs::path &filename, bool delta_only) const
    {
        fs::path path = platform::path->config_folder() / filename;

        // NOTE: there is no guarantee that `path` is located within `configFolder()`,
        // as `filename` may have been absolute.

        fs::create_directories(path.parent_path());
        json::JsonPrettyWriter doc(path);

        if (delta_only && this->composite_)
        {
            doc.write(this->recursive_delta(this->default_settings()));
        }
        else
        {
            doc.write(*this);
        }
    }

    SettingsStore SettingsStore::default_settings() const
    {
        if (this->directories_.size() > 1)
        {
            types::PathList base_path(this->directories_);
            base_path.erase(base_path.begin());

            SettingsStore default_settings(base_path);
            default_settings.load(this->filenames_);
            return default_settings;
        }
        else
        {
            return {};
        }
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
                value = value.get_kvmap()->get(element.as_string());
            }
            else if (element.is_integral() && value.is_valuelist())
            {
                value = value.get_valuelist()->get(element.as_sint());
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
                if (auto *next_map = std::get_if<types::KeyValueMapRef>(&next_value))
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

}  // namespace shared
