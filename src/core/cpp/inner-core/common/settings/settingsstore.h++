/// -*- c++ -*-
//==============================================================================
/// @file settingsstore.h++
/// @brief Read/write configuration from/to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/path.h++"
#include "types/value.h++"
#include "types/create-shared.h++"

#include <filesystem>
#include <vector>

namespace core
{
    /// Generic settings store to read/write configuration values from/to .json files.
    ///
    ///
    /// Usage:
    ///   * Create a new SettingsStore object, and load values from
    ///     `MyConfigFile.json` relative to the default search path returned by
    ///     `paths::settings_path()` (which in turn may be overriden by the
    ///     `"CONFIGPATH"` environment variable):
    ///         SettingsStore settings("MyConfigFile");
    ///
    ///   * Save any updated settings to file within the _first_ folder specified in the
    ///     settings path
    ///         settings.save_delta();

    class SettingsStore : public types::KeyValueMap,
                          public types::enable_create_shared<SettingsStore>
    {
    public:
        using ptr = std::shared_ptr<SettingsStore>;

        // protected:
        //     use_shared_scope();

    public:
        /// @brief
        ///     Constructor.
        /// @param[in] filenames
        ///     Absolute or relative path names from which to load initial settings.
        ///     @sa 'load()`.
        /// @param[in] directories
        ///     A list of folders in which to look for relative settings file
        ///     paths.  The default value is as returned from
        ///     `paths::settings_paths()`, and normally comprises two folders:
        ///      * A per-unit configuration folder that persists through
        ///        software upgrades, and
        ///      * a folder provided by the software itself which gets replaced
        ///        during upgrades.
        ///     This default search path may be overriden by the CONFIGPATH
        ///     environment variable.
        SettingsStore(const types::PathList &filenames = {},
                      const types::PathList &directories = platform::path->settings_paths());

        /// @brief
        ///     Constructor that loads from a file on creation.
        /// @param[in] filename
        ///     Absolute or relative path name from which to load initial settings.
        ///     @sa 'load()`.
        /// @param[in] directories
        ///     A list of folders in which to look for relative settings file paths.
        ///     @sa SettingsStore() default constructor.
        SettingsStore(const fs::path &filename,
                      const types::PathList &directories = platform::path->settings_paths());

    public:
        using types::enable_create_shared<SettingsStore>::create_shared;

        bool loaded() const;

        bool reload();

        /// @brief
        ///     Load settings from the specified file.
        /// @param[in] filename
        ///     Settings file to load.
        /// @param[in] update_filenames
        ///     Append this filenaem to files associated with this
        ///     `SettingsStore` instance.  If this is the first file loaded,
        ///     it also becomes the default filename to which updates are saved.
        ///
        /// The path name may be absolute or relative. If this does not include
        /// a suffix, ".json" is appended.  If the name is relative, settings
        /// are read and merged in from any and all matching files relative to
        /// each folder from the `directories`, as passed into the constructor.

        bool load(const fs::path &filename,
                  bool update_filenames = true);

        bool load(const std::vector<fs::path> &filenames,
                  bool update_filenames = true);

        /// @brief
        ///     Load and merge in settings from an absolute path.
        /// @param[in] abspath
        ///     Settings file to load.
        bool load_from(const fs::path &abspath);

        /// @brief
        ///     Save current settings to the default .json file for this store,
        ///     relative to the first directory in CONFIGPATH.
        ///
        /// @param[in] delta_only
        ///     Save only key/value pairs that are different from application
        ///     defaults, as loaded from settings files in
        ///     `core::platform::PathProvider::default_settings_paths()`.
        ///
        /// @param[in] use_temp_file
        ///     First save to a temporary file in the same folder, then move it
        ///     in place.  Doing so ensures integrity of the contents in case
        ///     this process is shut down while writing to the file.
        ///
        /// The filename is the same as that from which settings were loaded, but
        /// located in the first directory of the settings path (normally a
        /// unit-specific configuration folder). For instance, given the settings
        /// path `{ "/etc/local-settings", "/usr/share/app-settings" }`, where
        /// settings were originally loaded from `my-settings.json` (in either
        /// of these folders or merged in from both), then the updated settings
        /// will be saved in `"/etc/local-settings/my-settings.json"`.
        virtual void save(bool delta_only = true,
                          bool use_temp_file = true);


        /// @brief
        ///     Save current settings to `filename[.json]`.
        ///
        /// @param[in] filename
        ///     Filename in which settings are saved.
        ///
        /// @param[in] delta_only
        ///     Save only key/value pairs that are different from how they were
        ///     loaded from files in folders _other than_ the specified file.
        ///     The idea is that this file contains only unit-specific settings;
        ///     other settings are kept at their default values, and subject to
        ///     update with new software releases.
        ///
        /// @param[in] use_temp_file
        ///     First save to a temporary file in the same folder, then move it
        ///     in place.  Doing so ensures integrity of the contents in case
        ///     this process is shut down while writing to the file.
        ///
        /// Save the settings object to the specified `filename`, which may be
        /// absolute or relative. If the name is relative, settings are
        /// saved to the first folder returned by `paths::settings_paths()`,
        /// i.e.  the first folder specified in the `"CONFIGPATH"`
        /// environment variable or otherwise the default system-specific
        /// configuration folder.
        ///
        /// If the name does not include a suffix, ".json" is appended.
        ///
        void save_to(const fs::path &filename,
                     bool delta_only = true,
                     bool use_temp_file = true) const;

        fs::path filename() const;
        types::PathList filenames() const;
        types::PathList directories() const;

    private:
        types::Value extract_value(const types::ValueList &path,
                                   const types::Value &fallback = {}) const noexcept;

        /// @brief
        ///    Insert or update a key/value pair into a subtree within the settings store
        /// @param[in] path
        ///    Fully hierarchical path to the settings key
        /// @param[in] value
        ///    Setting value
        /// @param[in] save
        ///    Save updated settings to the default filename for this store.
        ///    @sa save_delta()

        std::pair<iterator, bool>
        insert_value(const std::vector<std::string> &path,
                     const types::Value &value,
                     bool save);

    private:
        void write_to(const fs::path &path,
                      bool delta_only) const;

        SettingsStore default_settings() const;

    private:
        bool composite_;
        types::PathList directories_;
        std::vector<fs::path> filenames_;
    };
}  // namespace core
