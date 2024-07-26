// -*- c++ -*-
//==============================================================================
/// @file vfs-local-provider.c++
/// @brief VFS service - native implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-linux-provider.h++"
#include "vfs-linux-removable-context.h++"

namespace platform::vfs::local
{
    constexpr auto SETTING_REMOVABLE = "removable";
    constexpr auto SETTING_PARTITION = "partition";
    constexpr auto SETTING_PARTITION_EXCLUDE = "exclude-types";
    constexpr auto SETTING_REMOVABLE_ROOT_DEFAULT = "/mnt/removable";

    LinuxProvider::LinuxProvider(const std::string &name,
                                 core::platform::ProviderPriority priority)
        : Super(name, priority)
    {
        core::types::Value setting = this->settings.get(SETTING_REMOVABLE);
        this->root = setting.get(SETTING_CXT_ROOT, "/mnt/removable").as_string();
        this->preexec = setting.get(SETTING_CXT_PREEXEC).as_string();
        this->postexec = setting.get(SETTING_CXT_POSTEXEC).as_string();
        this->writable = setting.get(SETTING_CXT_POSTEXEC, true).as_bool();
    }

    LinuxProvider::~LinuxProvider()
    {
        this->monitor.stop();
        this->monitor.signal_event.disconnect(this->name());
    }

    bool LinuxProvider::is_pertinent()
    {
        return this->monitor.available();
    }

    void LinuxProvider::initialize()
    {
        Super::initialize();

        this->monitor.init();
        using namespace std::placeholders;
        this->monitor
            .signal_event.connect(
                this->name(),
                std::bind(&LinuxProvider::processEvent, this, _1));

        volume::Enumerator enumerator;
        enumerator.init();
        while (volume::EventRef event = enumerator.next())
        {
            this->processEvent(*event);
        }

        this->monitor.start();
    }

    void LinuxProvider::processEvent(const volume::Event &event)
    {
        SignalHandler handler = nullptr;

        try
        {
            handler = LinuxProvider::volumeHandlers
                          .at(event.deviceType())
                          .at(event.actionType());
        }
        catch (const std::out_of_range &e)
        {
            logf_debug("Unhandled volume event: %s", event);
        }

        if (handler)
        {
            logf_trace("Invoking handler for event: %s", event);
            (this->*handler)(event);
        }
    }

    void LinuxProvider::on_disk_added(const volume::Event &event)
    {
        auto di = volume::DiskInfo(event);
        if (di.removable)
        {
            logf_debug("%s added", di);
        }
        this->disks.emplace(di.serial, di);
    }

    void LinuxProvider::on_disk_removed(const volume::Event &event)
    {
        auto di = volume::DiskInfo(event);
        if (di.removable)
        {
            logf_debug("%s removed", di);
        }
        this->disks.erase(di.serial);
    }

    void LinuxProvider::on_disk_changed(const volume::Event &event)
    {
        auto di = volume::DiskInfo(event);
        if (di.removable)
        {
            logf_debug("%s changed", di);
        }
    }

    void LinuxProvider::on_disk_online(const volume::Event &event)
    {
        auto di = volume::DiskInfo(event);
        if (di.removable)
        {
            logf_debug("%s online", di);
        }
    }

    void LinuxProvider::on_disk_offline(const volume::Event &event)
    {
        auto di = volume::DiskInfo(event);
        if (di.removable)
        {
            logf_debug("%s offline", di);
        }
    }

    void LinuxProvider::on_partition_added(const volume::Event &event)
    {
        auto pi = volume::PartitionInfo(event);
        logf_trace("%s added", pi);

        bool removable = false;
        auto it = this->disks.find(pi.serial);
        if (it != this->disks.end())
        {
            removable = it->second.removable;
        }

        // Validate patition type
        core::types::ValueList excludeTypes =
            settings.get(SETTING_PARTITION)
                .get(SETTING_PARTITION_EXCLUDE)
                .as_valuelist();

        bool validType = true;
        for (const auto &type : excludeTypes)
        {
            if (type.as_string() == pi.type)
            {
                validType = false;
                break;
            }
        }

        if (removable &&
            !pi.fstype.empty() &&
            validType)
        {
            this->register_partition(pi);
        }
    }

    void LinuxProvider::on_partition_removed(const volume::Event &event)
    {
        auto pi = volume::PartitionInfo(event);
        logf_trace("%s removed", pi);
        this->unregister_partition(pi);
    }

    void LinuxProvider::on_partition_changed(const volume::Event &event)
    {
        auto pi = volume::PartitionInfo(event);
        logf_trace("%s changed", pi);
    }

    void LinuxProvider::on_partition_online(const volume::Event &event)
    {
        auto pi = volume::PartitionInfo(event);
        logf_trace("%s online", pi);
    }

    void LinuxProvider::on_partition_offline(const volume::Event &event)
    {
        auto pi = volume::PartitionInfo(event);
        logf_trace("%s offline", pi);
    }

    void LinuxProvider::register_partition(const volume::PartitionInfo &pi)
    {
        auto cxt = std::make_shared<RemovableContext>(
            pi.sysname,
            this->root / pi.sysname,
            pi.writable && this->writable,
            this->preexec,
            this->postexec,
            pi.friendlyName(),
            pi.devnode,
            pi.fstype);

        logf_debug("Registering partition %s", pi.sysname);
        this->addContext(pi.sysname, cxt);
    }

    void LinuxProvider::unregister_partition(const volume::PartitionInfo &pi)
    {
        this->removeContext(pi.sysname);
    }

    std::string LinuxProvider::newContextName(const std::string &basename)
    {
        std::string suffix;
        for (int attempt = 1;
             this->contexts.count(basename + suffix) > 0;
             attempt++)
        {
            suffix = core::str::format(" %d", attempt);
        }
        return basename + suffix;
    }

    LinuxProvider::VolumeHandlerMap LinuxProvider::volumeHandlers = {
        {volume::DEVTYPE_DISK,
         {
             {volume::ACTION_ADD, &LinuxProvider::on_disk_added},
             {volume::ACTION_REMOVE, &LinuxProvider::on_disk_removed},
             {volume::ACTION_CHANGE, &LinuxProvider::on_disk_changed},
             {volume::ACTION_ONLINE, &LinuxProvider::on_disk_online},
             {volume::ACTION_OFFLINE, &LinuxProvider::on_disk_offline},
         }},
        {volume::DEVTYPE_PARTITION,
         {
             {volume::ACTION_ADD, &LinuxProvider::on_partition_added},
             {volume::ACTION_REMOVE, &LinuxProvider::on_partition_removed},
             {volume::ACTION_CHANGE, &LinuxProvider::on_partition_changed},
             {volume::ACTION_ONLINE, &LinuxProvider::on_partition_online},
             {volume::ACTION_OFFLINE, &LinuxProvider::on_partition_offline},
         }},
    };
}  // namespace platform::vfs::local
