// -*- c++ -*-
//==============================================================================
/// @file vfs-local-provider.h++
/// @brief VFS service - native implementation
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "vfs-base.h++"
#include "settings/settingsstore.h++"

namespace vfs::local
{
    constexpr auto SETTINGS_STORE = "vfs";
    constexpr auto SETTING_CONTEXTS = "contexts";
    constexpr auto SETTING_CXT_ROOT = "root";
    constexpr auto SETTING_CXT_PREEXEC = "preexec";
    constexpr auto SETTING_CXT_POSTEXEC = "postexec";
    constexpr auto SETTING_CXT_WRITABLE = "writable";
    constexpr auto SETTING_CXT_TITLE = "title";

    //==========================================================================
    // NativeProvider

    class LocalProvider : public ProviderInterface
    {
        using This = LocalProvider;
        using Super = ProviderInterface;

    protected:
        LocalProvider(
            const std::string &name = "LocalProvider",
            core::platform::ProviderPriority priority = core::platform::PRIORITY_LOW);

    public:
        void initialize() override;

    public:
        ContextMap get_contexts() const override;
        ContextMap get_open_contexts() const override;
        Context::ptr get_context(const std::string &name, bool required) const override;
        Context::ptr open_context(const std::string &name, bool required) override;
        void close_context(const std::string &name, bool required) override;
        void close_context(const Context::ptr &cxt) override;

        VolumeInfo get_volume_info(const Path &vpath,
                                   const OperationFlags &flags) const override;

        FileInfo get_file_info(const Path &vpath,
                               const OperationFlags &flags) const override;

        Directory get_directory(const Path &vpath,
                                const OperationFlags &flags) const override;

        Directory locate(const Path &vpath,
                         const core::types::PathList &filename_masks,
                         const core::types::TaggedValueList &attribute_filters,
                         const OperationFlags &flags) const override;

        void copy(const Paths &sources,
                  const Path &target,
                  const OperationFlags &flags) const override;

        void move(const Paths &sources,
                  const Path &target,
                  const OperationFlags &flags) const override;

        void remove(const Paths &vpaths,
                    const OperationFlags &flags) const override;

        void create_folder(const Path &vpath,
                           const OperationFlags &flags) const override;

        UniqueReader read_file(const Path &vpath) const override;

        UniqueWriter write_file(const Path &vpath) const override;

        core::types::KeyValueMap get_attributes(const Path &vpath) const override;

        void set_attributes(const Path &vpath,
                            const core::types::KeyValueMap &attributes) const override;

        void clear_attributes(const Path &vpath) const override;

    protected:
        void addContext(const std::string &name, Context::ptr cxt);
        bool removeContext(const std::string &name);

    private:
        void loadContexts(void);
        Context::ptr newContext(
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

        virtual bool attribute_match(
            const core::types::TaggedValueList &attribute_filters,
            const core::types::KeyValueMap &attributes) const;

        virtual core::types::KeyValueMap get_attributes(
            const fs::path &localpath,
            fs::file_type type_hint = fs::file_type::none) const;

        virtual void set_attributes(
            const fs::path &localpath,
            const core::types::KeyValueMap &attributes,
            fs::file_type pathtype = fs::file_type::none,
            bool save = true) const;

        virtual void clear_attributes(
            const fs::path &localpath,
            fs::file_type type_hint = fs::file_type::none,
            bool save = true) const;

    public:  //  Data
        ContextMap contexts;

    protected:
        core::SettingsStore settings;
    };

}  // namespace vfs::local
