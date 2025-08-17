// -*- c++ -*-
//==============================================================================
/// @file vfs-attribute-store.h++
/// @brief VFS service - attribute storage for local VFS contexts
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-local-context.h++"
#include "vfs-local-fileio.h++"
#include "settings/settingsstore.h++"
#include "types/filesystem.h++"

namespace vfs::local
{

    //==========================================================================
    /// Attribute store.
    class AttributeStore : public core::SettingsStore
    {
        using Super = core::SettingsStore;

    public:
        AttributeStore(const fs::path &path,
                       fs::file_type type_hint = fs::file_type::none);

        fs::file_type file_type(const fs::path &path,
                                fs::file_type hint);

        fs::path path(const fs::path &path);

        std::string key(const fs::path &basename = {});

        core::types::KeyValueMap get_attributes(const fs::path &basename = {});
        core::types::Value get_attribute(const fs::path &basename,
                                         const std::string &key);

        void set_attributes(const fs::path &basename,
                            const core::types::KeyValueMap &map,
                            bool save = true);

        void set_attributes(const core::types::KeyValueMap &map,
                            bool save = true);

        void clear_attributes(const fs::path &basename = {},
                              bool save = true);

    protected:
        void save(bool delta = true,
                  bool use_temp_file = true) override;
        void prune();

    private:
        fs::file_type filetype;
        fs::path filepath;
        bool loaded;
    };
}  // namespace vfs::local
