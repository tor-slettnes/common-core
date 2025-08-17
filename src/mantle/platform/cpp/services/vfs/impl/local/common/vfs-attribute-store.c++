// -*- c++ -*-
//==============================================================================
/// @file vfs-attribute-store.c++
/// @brief VFS service - attribute storage for local VFS contexts
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-attribute-store.h++"

namespace vfs::local
{
    constexpr auto ATTRIBUTE_STORE = ".attributes";
    constexpr auto DIRECTORY_ENTRY = ".";

    AttributeStore::AttributeStore(const fs::path &path,
                                   fs::file_type type_hint)
        : SettingsStore(),
          filetype(this->file_type(path, type_hint)),
          filepath(path)
    {
        this->loaded = this->load(this->path(path));
    }

    fs::file_type AttributeStore::file_type(const fs::path &path,
                                            fs::file_type hint)
    {
        if (hint == fs::file_type::none)
        {
            return fs::status(path).type();
        }
        else
        {
            return hint;
        }
    }

    fs::path AttributeStore::path(const fs::path &path)
    {
        if (this->filetype == fs::file_type::directory)
        {
            return path / ATTRIBUTE_STORE;
        }
        else
        {
            return path.parent_path() / ATTRIBUTE_STORE;
        }
    }

    std::string AttributeStore::key(const fs::path &basename)
    {
        if (!basename.empty())
        {
            return basename.string();
        }
        else if (this->filetype == fs::file_type::directory)
        {
            return DIRECTORY_ENTRY;
        }
        else
        {
            return this->filepath.filename().string();
        }
    }

    core::types::KeyValueMap AttributeStore::get_attributes(const fs::path &basename)
    {
        return this->get(this->key(basename)).as_kvmap();
    }

    core::types::Value AttributeStore::get_attribute(const fs::path &basename,
                                                     const std::string &key)
    {
        return this->get(this->key(basename)).get(key);
    }

    void AttributeStore::set_attributes(const fs::path &basename,
                                        const core::types::KeyValueMap &map,
                                        bool save)

    {
        if (core::types::KeyValueMapPtr attrs = this->get(this->key(basename)).get_kvmap_ptr())
        {
            attrs->update(map);
        }
        else
        {
            this->insert_or_assign(this->key(basename), map);
        }

        if (save)
        {
            this->save();
        }
    }

    void AttributeStore::set_attributes(const core::types::KeyValueMap &map,
                                        bool save)

    {
        this->set_attributes({}, map, save);
    }

    void AttributeStore::clear_attributes(const fs::path &basename, bool save)
    {
        if (this->erase(this->key(basename)))
        {
            if (save)
            {
                this->save();
            }
        }
    }

    void AttributeStore::save(bool delta, bool use_temp_file)
    {
        this->prune();

        if (!this->empty())
        {
            Super::save(delta, use_temp_file);
            this->loaded = true;
        }
        else if (this->loaded)
        {
            std::error_code ec;
            fs::remove(this->filename(), ec);
            this->loaded = false;
        }
    }

    void AttributeStore::prune()
    {
        std::unordered_set<std::string> extraneous_names;
        extraneous_names.reserve(this->size());
        for (auto &[key, value] : *this)
        {
            if (key != DIRECTORY_ENTRY)
            {
                extraneous_names.insert(key);
            }
        }

        for (auto pi : fs::directory_iterator(this->filename().parent_path()))
        {
            extraneous_names.erase(pi.path().filename().string());
        }

        for (const fs::path &extraneous_name : extraneous_names)
        {
            this->erase(extraneous_name);
        }
    }
}  // namespace vfs::local
