// -*- c++ -*-
//==============================================================================
/// @file udev-event.c++
/// @brief Monitor UDev events - Event type
/// @author Tor Slettnes
//==============================================================================

#include "udev-deviceinfo.h++"
#include "types/symbolmap.h++"
#include "types/valuemap.h++"
#include "logging/logging.h++"

#include <thread>
#include <chrono>
#include <memory>

#define UDEV_DEVTYPE_BLOCK "block"
#define UDEV_DEVTYPE_TTY   "tty"
#define BLOCK_SIZE         512
#define PROPERTY_ACTION    "ACTION"

namespace cc::core::udev
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
        {ACTION_BIND, "bind"},
        {ACTION_UNBIND, "unbind"},
        {ACTION_ADD, "add"},
        {ACTION_REMOVE, "remove"},
        {ACTION_CHANGE, "change"},
        {ACTION_ONLINE, "online"},
        {ACTION_OFFLINE, "offline"}};

    std::ostream &operator<<(std::ostream &stream, ActionType action_type)
    {
        return actiontype_map.to_stream(stream, action_type);
    }

    std::istream &operator>>(std::istream &stream, ActionType &action_type)
    {
        return actiontype_map.from_stream(stream, &action_type);
    }

    const core::types::SymbolMap<DeviceType> devicetype_map = {
        {DEVTYPE_TTY, "tty"},
        {DEVTYPE_DISK, "disk"},
        {DEVTYPE_PARTITION, "partition"},
        {DEVTYPE_LOOP, "loop"},
    };

    std::ostream &operator<<(std::ostream &stream, DeviceType device_type)
    {
        return devicetype_map.to_stream(stream, device_type);
    }

    std::istream &operator>>(std::istream &stream, DeviceType &device_type)
    {
        return devicetype_map.from_stream(stream, &device_type);
    }

    //--------------------------------------------------------------------------
    // UDev DeviceInfo

    DeviceInfo::DeviceInfo(struct udev_device *device, bool borrowed)
        : device(device),
          borrowed(borrowed)
    {
        logf_trace("UDev event: %r", *this);
    }

    DeviceInfo::~DeviceInfo()
    {
        if (this->device && !this->borrowed)
        {
            udev_device_unref(this->device);
        }
    }

    ActionType DeviceInfo::actionType() const noexcept
    {
        return core::str::convert_to<ActionType>(this->action(), ACTION_UNKNOWN);
    }

    DeviceType DeviceInfo::deviceType() const noexcept
    {
        return core::str::convert_to<DeviceType>(this->devtype(), DEVTYPE_UNKNOWN);
    }

    std::shared_ptr<DeviceInfo> DeviceInfo::parent_device() const
    {
        if (auto parent = udev_device_get_parent(this->device))
        {
            return DeviceInfo::create_shared(parent, true);
        }
        else
        {
            return {};
        }
    }

    std::string DeviceInfo::action() const
    {
        return udev_device_get_action(this->device);
    }

    std::string DeviceInfo::devpath() const
    {
        return udev_device_get_devpath(this->device);
    }

    std::string DeviceInfo::subsystem() const
    {
        return udev_device_get_subsystem(this->device);
    }

    std::string DeviceInfo::devtype() const
    {
        return udev_device_get_devtype(this->device);
    }

    std::string DeviceInfo::syspath() const
    {
        return udev_device_get_syspath(this->device);
    }

    std::string DeviceInfo::sysname() const
    {
        return udev_device_get_sysname(this->device);
    }

    std::string DeviceInfo::sysnum() const
    {
        return udev_device_get_sysnum(this->device);
    }

    std::string DeviceInfo::devnode() const
    {
        return udev_device_get_devnode(this->device);
    }

    std::string DeviceInfo::driver() const
    {
        return udev_device_get_driver(this->device);
    }

    dev_t DeviceInfo::devnum() const
    {
        return udev_device_get_devnum(this->device);
    }

    unsigned long long int DeviceInfo::seqnum() const
    {
        return udev_device_get_seqnum(this->device);
    }

    core::dt::Duration DeviceInfo::age() const
    {
        return std::chrono::microseconds(
            udev_device_get_usec_since_initialized(this->device));
    }

    DeviceInfo::AttributeMap DeviceInfo::properties() const
    {
        return This::attribute_map(udev_device_get_properties_list_entry(this->device));
    }

    DeviceInfo::AttributeList DeviceInfo::devlinks() const
    {
        return This::attribute_list(udev_device_get_devlinks_list_entry(this->device));
    }

    DeviceInfo::AttributeSet DeviceInfo::tags() const
    {
        return This::attribute_set(udev_device_get_tags_list_entry(this->device));
    }

    DeviceInfo::AttributeMap DeviceInfo::system_attributes() const
    {
        return This::attribute_map(udev_device_get_sysattr_list_entry(this->device));
    }

    void DeviceInfo::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->extend({
            {"action", this->action()},
            {"actionType", core::str::convert_from(this->actionType())},
            {"devicetype", core::str::convert_from(this->deviceType())},
            {"devpath", this->devpath()},
            {"subsystem", this->subsystem()},
            {"devtype", this->devtype()},
            {"syspath", this->syspath()},
            {"sysname", this->sysname()},
            {"sysnum", this->sysnum()},
            {"devnode", this->devnode()},
            {"driver", this->driver()},
            {"devnum", this->devnum()},
            {"seqnum", this->seqnum()},
            {"age", this->age()},
            {"properties",
             core::types::KeyValueMap::create_from(this->properties())},
            {"devlinks",
             core::types::ValueList::create_from(this->devlinks())},
            {"tags",
             core::types::ValueList::create_from(this->tags())},
            {"sysattrs",
             core::types::KeyValueMap::create_from(this->system_attributes())},
        });
    }

    DeviceInfo::AttributeMap DeviceInfo::attribute_map(struct udev_list_entry *list) noexcept
    {
        AttributeMap map;

        for (struct udev_list_entry *entry = list;
             entry != nullptr;
             entry = udev_list_entry_get_next(entry))
        {
            map.insert_or_assign(
                udev_list_entry_get_name(entry),
                udev_list_entry_get_value(entry));
        }
        return map;
    }

    DeviceInfo::AttributeList DeviceInfo::attribute_list(struct udev_list_entry *list) noexcept
    {
        AttributeList attributes;

        for (struct udev_list_entry *entry = list;
             entry != nullptr;
             entry = udev_list_entry_get_next(entry))
        {
            attributes.push_back(udev_list_entry_get_name(entry));
        }
        return attributes;
    }

    DeviceInfo::AttributeSet DeviceInfo::attribute_set(struct udev_list_entry *list) noexcept
    {
        AttributeSet set;

        for (struct udev_list_entry *entry = list;
             entry != nullptr;
             entry = udev_list_entry_get_next(entry))
        {
            set.insert(udev_list_entry_get_name(entry));
        }
        return set;
    }

    std::string DeviceInfo::string(const char *cstr) noexcept
    {
        return cstr ? cstr : "";
    }

}  // namespace cc::core::udev
