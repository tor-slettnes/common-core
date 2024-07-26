// -*- c++ -*-
//==============================================================================
/// @file vfs-local-provider.h++
/// @brief VFS service - native implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-provider.h++"
#include "settings/settingsstore.h++"

namespace platform::vfs::local
{
    constexpr auto SETTINGS_STORE = "vfs.json";
    constexpr auto SETTING_CONTEXTS = "contexts";
    constexpr auto SETTING_CXT_ROOT = "root";
    constexpr auto SETTING_CXT_PREEXEC = "preexec";
    constexpr auto SETTING_CXT_POSTEXEC = "postexec";
    constexpr auto SETTING_CXT_WRITABLE = "writable";
    constexpr auto SETTING_CXT_TITLE = "title";

    //==========================================================================
    // NativeProvider

    class LocalProvider : public platform::vfs::Provider
    {
        using Super = platform::vfs::Provider;

    protected:
        LocalProvider(
            const std::string &name = "LocalProvider",
            core::platform::ProviderPriority priority = core::platform::PRIORITY_LOW);

    public:
        void initialize() override;

    public:
        ContextMap getContexts() const override;

        ContextMap getOpenContexts() const override;

        ContextRef getContext(const std::string &name, bool required) const override;

        ContextRef openContext(const std::string &name, bool required) override;

        void closeContext(const ContextRef &cxt) override;

        VolumeStats volumeStats(const Path &vpath,
                                const OperationFlags &flags) const override;

        FileStats fileStats(const Path &vpath,
                            const OperationFlags &flags) const override;

        Directory getDirectory(const Path &vpath,
                               const OperationFlags &flags) const override;

        Directory locate(const Path &vpath,
                         const std::vector<std::string> &filename_masks,
                         const core::types::TaggedValueList &attribute_filters,
                         const OperationFlags &flags) const override;

        void copy(const Paths &sources,
                  const Path &target,
                  const OperationFlags &flags) const override;

        void move(const Paths &sources,
                  const Path &target,
                  const OperationFlags &flags) const override;

        void remove(const Paths &sources,
                    const OperationFlags &flags) const override;

        void createFolder(const Path &vpath,
                          const OperationFlags &flags) const override;

        ReaderRef readFile(const Path &vpath) const override;

        WriterRef writeFile(const Path &vpath) const override;

        core::types::KeyValueMap getAttributes(const Path &vpath) const override;

        void setAttributes(const Path &vpath,
                           const core::types::KeyValueMap &attributes) const override;

        void clearAttributes(const Path &vpath) const override;

    protected:
        void addContext(const std::string &name, ContextRef cxt);
        bool removeContext(const std::string &name);

    private:
        void loadContexts(void);
        ContextRef newContext(
            const std::string &name,
            const core::types::Value &settings);

    protected:
        virtual void copy2(
            const Location &srcloc,
            const Location &tgtloc,
            const OperationFlags &flags) const;

        virtual void move2(
            const Location &srcloc,
            const Location &tgtloc,
            const OperationFlags &flags) const;

        virtual void remove(
            const Location &loc,
            const OperationFlags &flags) const;

        virtual FileStats read_stats(
            const fs::path &localpath,
            bool dereference = false) const;

        virtual void locate_inside(
            const fs::path &root,
            const fs::path &relpath,
            const std::vector<std::string> &filename_masks,
            const core::types::TaggedValueList &attribute_filters,
            bool with_attributes,
            bool include_hidden,
            bool ignore_case,
            Directory *dir) const;

        virtual bool filename_match(
            const std::vector<std::string> &masks,
            const fs::path &basename,
            bool include_hidden,
            bool ignore_case) const;

        virtual bool attribute_match(
            const core::types::TaggedValueList &attribute_filters,
            const core::types::KeyValueMap &attributes) const;

        virtual core::types::KeyValueMap getAttributes(
            const fs::path &localpath,
            fs::file_type type_hint = fs::file_type::none) const;

        virtual void setAttributes(
            const fs::path &localpath,
            const core::types::KeyValueMap &attributes,
            fs::file_type pathtype = fs::file_type::none,
            bool save = true) const;

        virtual void clearAttributes(
            const fs::path &localpath,
            fs::file_type type_hint = fs::file_type::none,
            bool save = true) const;

    public:  //  Data
        ContextMap contexts;

    protected:
        core::SettingsStore settings;
    };

}  // namespace platform::vfs::local
