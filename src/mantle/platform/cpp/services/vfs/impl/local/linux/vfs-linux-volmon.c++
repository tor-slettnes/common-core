// -*- c++ -*-
//==============================================================================
/// @file vfs-linux-volmon.c++
/// @brief VFS service - Monitor filesystem evens via Linux udev
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-linux-volmon.h++"
#include "types/symbolmap.h++"
#include "types/valuemap.h++"
#include "logging/logging.h++"

#include <thread>
#include <chrono>
#include <memory>

namespace platform::vfs::local::volume
{
    constexpr auto PROP_TYPE = "ID_PART_ENTRY_TYPE";
    constexpr auto PROP_SERIAL = "ID_SERIAL";
    constexpr auto PROP_LABEL = "ID_FS_LABEL_ENC";
    constexpr auto PROP_FSTYPE = "ID_FS_TYPE";
    constexpr auto PROP_UUID = "ID_FS_UUID";
    constexpr auto PROP_MODEL = "ID_MODEL_ENC";
    constexpr auto PROP_VENDOR = "ID_VENDOR_ENC";
    constexpr auto PROP_BUS = "ID_BUS";
    constexpr auto ATTR_READONLY = "ro";
    constexpr auto ATTR_REMOVABLE = "removable";
    constexpr auto ATTR_PART = "partition";

    const core::types::SymbolMap<ActionType> actiontype_map = {
        {ACTION_ADD, "add"},
        {ACTION_REMOVE, "remove"},
        {ACTION_CHANGE, "change"},
        {ACTION_ONLINE, "online"},
        {ACTION_OFFLINE, "offline"}};

    const core::types::SymbolMap<DeviceType> devicetype_map = {
        {DEVTYPE_DISK, "disk"},
        {DEVTYPE_PARTITION, "partition"},
        {DEVTYPE_LOOP, "loop"},
    };

    const core::types::ValueMap<ParameterType, DeviceLookupMethod> udev_device_lookup_map = {
        {PARAM_DRIVERS, &udev_device_get_driver},
        {PARAM_SUBSYSTEM, &udev_device_get_subsystem},
        {PARAM_DEVTYPE, &udev_device_get_devtype},
        {PARAM_SYSPATH, &udev_device_get_syspath},
        {PARAM_SYSNAME, &udev_device_get_sysname},
        {PARAM_SYSNUM, &udev_device_get_sysnum},
        {PARAM_DEVNODE, &udev_device_get_devnode},
    };

    //==========================================================================
    /// Volume Base - UDev Device Events

    Event::Event(struct udev_device *device, ActionType actiontype)
        : action(string(udev_device_get_action(device))),
          devpath(string(udev_device_get_devpath(device))),
          subsystem(string(udev_device_get_subsystem(device))),
          devtype(string(udev_device_get_devtype(device))),
          syspath(string(udev_device_get_syspath(device))),
          sysname(string(udev_device_get_sysname(device))),
          sysnum(string(udev_device_get_sysnum(device))),
          devnode(string(udev_device_get_devnode(device))),
          driver(string(udev_device_get_driver(device))),
          devnum(udev_device_get_devnum(device)),
          seqnum(udev_device_get_seqnum(device)),
          age(std::chrono::microseconds(udev_device_get_usec_since_initialized(device))),
          properties(valuemap(udev_device_get_properties_list_entry(device))),
          devlinks(valuelist(udev_device_get_devlinks_list_entry(device))),
          tags(valueset(udev_device_get_tags_list_entry(device))),
          sysattrs(systemattributes(device)),
          is_usb((find_parent(device, PARAM_DRIVERS, "usb-storage") != nullptr)),
          actionType_(actiontype)
    {
        logf_trace("Volume event: %r", *this);
    }

    ActionType Event::actionType() const noexcept
    {
        return (this->actionType_ != ACTION_UNKNOWN)
                   ? this->actionType_
                   : actiontype_map.from_string(this->action, ACTION_UNKNOWN);
    }

    DeviceType Event::deviceType() const noexcept
    {
        return devicetype_map.from_string(this->devtype, DEVTYPE_UNKNOWN);
    }

    void Event::to_stream(std::ostream &stream) const
    {
        stream << "{action=" << std::quoted(this->action)
               << " (type " << this->actionType() << ")"
               << ", devicetype=" << this->deviceType()
               << ", devpath=" << std::quoted(this->devpath)
               << ", subsystem=" << std::quoted(this->subsystem)
               << ", devtype=" << std::quoted(this->devtype)
               << ", syspath=" << std::quoted(this->syspath)
               << ", sysname=" << std::quoted(this->sysname)
               << ", sysnum=" << std::quoted(this->sysnum)
               << ", devnode=" << std::quoted(this->devnode)
               << ", driver=" << std::quoted(this->driver)
               << ", devnum=" << this->devnum
               << ", seqnum=" << this->seqnum
               << ", age=" << this->age
               << ", properties=" << this->properties
               << ", devlinks=" << this->devlinks
               << ", tags=" << this->tags
               << ", sysattrs=" << this->sysattrs
               << "}";
    }

    Event::KeyValueMap Event::valuemap(struct udev_list_entry *list) noexcept
    {
        KeyValueMap map;

        for (struct udev_list_entry *entry = list;
             entry != nullptr;
             entry = udev_list_entry_get_next(entry))
        {
            map.insert_or_assign(
                std::string(udev_list_entry_get_name(entry)),
                std::string(udev_list_entry_get_value(entry)));
        }
        return map;
    }

    Event::ValueList Event::valuelist(struct udev_list_entry *list) noexcept
    {
        ValueList values;

        for (struct udev_list_entry *entry = list;
             entry != nullptr;
             entry = udev_list_entry_get_next(entry))
        {
            values.push_back(udev_list_entry_get_name(entry));
        }
        return values;
    }

    Event::ValueSet Event::valueset(struct udev_list_entry *list) noexcept
    {
        ValueSet values;

        for (struct udev_list_entry *entry = list;
             entry != nullptr;
             entry = udev_list_entry_get_next(entry))
        {
            values.insert(udev_list_entry_get_name(entry));
        }
        return values;
    }

    Event::KeyValueMap Event::systemattributes(struct udev_device *device) noexcept
    {
        KeyValueMap map;

        for (struct udev_list_entry *entry = udev_device_get_sysattr_list_entry(device);
             entry != nullptr;
             entry = udev_list_entry_get_next(entry))
        {
            const char *name = udev_list_entry_get_name(entry);
            if (name)
            {
                map[name] = Event::string(udev_device_get_sysattr_value(device, name));
            }
        }
        return map;
    }

    std::string Event::string(const char *cstr) noexcept
    {
        return cstr ? cstr : "";
    }

    udev_device *Event::find_parent(udev_device *device,
                                    ParameterType parameter,
                                    const std::string &value) noexcept
    {
        if (auto handler = udev_device_lookup_map.get(parameter))
        {
            while (auto parent = udev_device_get_parent(device))
            {
                if (Event::string(handler(parent)) == value)
                {
                    return parent;
                }
                device = parent;
            }
        }

        return nullptr;
    }

    //==========================================================================
    // Disk Info

    DiskInfo::DiskInfo(const Event &event)
        : devnode(event.devnode),
          serial(event.properties.get(PROP_SERIAL).as_string()),
          removable(event.is_usb),
          writable(!event.sysattrs.get(ATTR_READONLY, false).as_bool())
    {
    }

    void DiskInfo::to_stream(std::ostream &stream) const
    {
        core::str::format(
            stream,
            "Disk(devnode=%r, serial=%r, removable=%b, writable=%b)",
            this->devnode,
            this->serial,
            this->removable,
            this->writable);
    }

    bool DiskInfo::is_removable(const Event &event)
    {
        return event.is_usb;
    }

    //==========================================================================
    // Disk Info

    PartitionInfo::PartitionInfo(const Event &event)
        : devnode(event.devnode),
          sysname(event.sysname),
          type(event.properties.get(PROP_TYPE).as_string()),
          serial(event.properties.get(PROP_SERIAL).as_string()),
          label(core::str::strip(
              core::str::unescaped(
                  event.properties.get(PROP_LABEL).as_string()))),
          fstype(event.properties.get(PROP_FSTYPE).as_string()),
          uuid(event.properties.get(PROP_UUID).as_string()),
          vendor(core::str::strip(
              core::str::unescaped(
                  event.properties.get(PROP_VENDOR).as_string()))),
          model(core::str::strip(
              core::str::unescaped(
                  event.properties.get(PROP_MODEL).as_string()))),
          partnumber(event.sysattrs.get(ATTR_PART, 0).as_uint()),
          writable(!event.sysattrs.get(ATTR_READONLY, false).as_bool())
    {
    }

    void PartitionInfo::to_stream(std::ostream &stream) const
    {
        core::str::format(
            stream,
            "Partition(devnode=%r, sysname=%r, uuid=%r, serial=%r, "
            "label=%r, fstype=%r, vendor=%r, model=%r, partition=%d)",
            this->devnode,
            this->sysname,
            this->uuid,
            this->serial,
            this->label,
            this->fstype,
            this->vendor,
            this->model,
            this->partnumber);
    }

    std::string PartitionInfo::friendlyName() const
    {
        std::string name = this->label;
        if (name.empty())
            name = this->model;
        if (name.empty())
            name = this->serial;
        if (name.empty())
            name = this->uuid;
        if (name.empty())
            name = "USB Drive";
        return name;
    }

    //==========================================================================
    /// Volume Base - Base for Enumerator and Monitor

    Base::Base()
        : udev(udev_new())
    {
    }

    Base::~Base()
    {
        udev_device_unref(this->device);
        udev_unref(this->udev);
    }

    bool Base::available() const
    {
        return bool(this->udev);
    }

    Event::ptr Base::next()
    {
        this->device = udev_device_unref(this->device);
        return {};
    }

    //==========================================================================
    /// Volume Enumerator - Obtain availabled over devices in system

    Enumerator::Enumerator()
        : Base(),
          enumerator(udev_enumerate_new(this->udev))
    {
    }

    Enumerator::~Enumerator()
    {
        udev_enumerate_unref(this->enumerator);
    }

    void Enumerator::init()
    {
        Base::init();
        udev_enumerate_add_match_subsystem(this->enumerator, UDEV_DEVTYPE);
        udev_enumerate_scan_devices(this->enumerator);
        this->devices = udev_enumerate_get_list_entry(this->enumerator);
        this->dev_list_entry = this->devices;
    }

    Event::ptr Enumerator::next()
    {
        Base::next();

        if (this->dev_list_entry)
        {
            const char *path = udev_list_entry_get_name(this->dev_list_entry);
            this->device = udev_device_new_from_syspath(this->udev, path);
            this->dev_list_entry = udev_list_entry_get_next(this->dev_list_entry);
            return std::make_shared<Event>(this->device, ACTION_ADD);
        }

        return {};
    }

    //==========================================================================
    /// Volume Monitor - monitor additions/removal/changes in udev

    Monitor::Monitor()
        : Base(),
          signal_event("volume monitor event"),
          monitor(udev_monitor_new_from_netlink(this->udev, "udev"))
    {
    }

    Monitor::~Monitor()
    {
        udev_monitor_unref(this->monitor);
    }

    bool Monitor::available() const
    {
        return Base::available() && bool(this->monitor);
    }

    void Monitor::init()
    {
        Base::init();
        udev_monitor_filter_add_match_subsystem_devtype(this->monitor, UDEV_DEVTYPE, NULL);
        udev_monitor_enable_receiving(this->monitor);
        this->fd = udev_monitor_get_fd(this->monitor);
    }

    void Monitor::start()
    {
        if (!this->runthread.joinable())
        {
            this->running = true;
            this->runthread = std::thread(&Monitor::run, this);
        }
    }

    void Monitor::stop()
    {
        this->running = false;
        if (this->runthread.joinable())
        {
            this->runthread.join();
        }
    }

    void Monitor::run()
    {
        while (auto event = this->next())
        {
            this->signal_event.emit(*event);
        }
    }

    Event::ptr Monitor::next()
    {
        Base::next();

        fd_set fds;
        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        while (this->running)
        {
            FD_ZERO(&fds);
            FD_SET(this->fd, &fds);

            if (select(this->fd + 1, &fds, NULL, NULL, &timeout) > 0 &&
                FD_ISSET(this->fd, &fds))
            {
                this->device = udev_monitor_receive_device(this->monitor);
                if (this->device)
                {
                    return std::make_shared<Event>(this->device);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return {};
    }
}  // namespace platform::vfs::local::volume
