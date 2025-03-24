// -*- c++ -*-
//==============================================================================
/// @file vfs-linux-provider.h++
/// @brief VFS service - native implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-local-provider.h++"
#include "vfs-linux-volmon.h++"
#include "types/valuemap.h++"

namespace vfs::local
{
    class LinuxProvider : public LocalProvider
    {
        using This = LinuxProvider;
        using Super = LocalProvider;

        using SignalHandler = void (This::*)(const volume::Event &event);
        using ActionHandlerMap = core::types::ValueMap<volume::ActionType, SignalHandler>;
        using VolumeHandlerMap = core::types::ValueMap<volume::DeviceType, ActionHandlerMap>;
        using SerialNumber = std::string;

    protected:
        LinuxProvider(
            const std::string &name = "LinuxProvider",
            core::platform::ProviderPriority priority = core::platform::PRIORITY_HIGH);

        ~LinuxProvider();

    public:
        bool is_pertinent() const override;
        void initialize() override;

    protected:
        void processEvent(const volume::Event &event);
        void on_disk_added(const volume::Event &event);
        void on_disk_removed(const volume::Event &event);
        void on_disk_changed(const volume::Event &event);
        void on_disk_online(const volume::Event &event);
        void on_disk_offline(const volume::Event &event);
        void on_partition_added(const volume::Event &event);
        void on_partition_removed(const volume::Event &event);
        void on_partition_changed(const volume::Event &event);
        void on_partition_online(const volume::Event &event);
        void on_partition_offline(const volume::Event &event);

    private:
        void register_partition(const volume::PartitionInfo &pi);
        void unregister_partition(const volume::PartitionInfo &pi);

        std::string newContextName(const std::string &basename);

        Context::ptr newContext(const std::string &name,
                                const core::types::Value &settings,
                                bool writeable);

    private:
        static VolumeHandlerMap volumeHandlers;
        core::SettingsStore settings;
        fs::path root;
        std::string preexec, postexec;
        bool writable;

        volume::Monitor monitor;
        core::types::ValueMap<SerialNumber, volume::DiskInfo> disks;
    };

}  // namespace vfs::local
