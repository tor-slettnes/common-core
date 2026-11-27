// -*- c++ -*-
//==============================================================================
/// @file udev-event.h++
/// @brief Monitor UDev events - DeviceInfo type
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/value.h++"
#include "types/listable.h++"
#include "types/create-shared.h++"

#include <libudev.h>

namespace cc::core::udev
{
    enum ActionType
    {
        ACTION_UNKNOWN,
        ACTION_BIND,
        ACTION_UNBIND,
        ACTION_ADD,
        ACTION_REMOVE,
        ACTION_CHANGE,
        ACTION_ONLINE,
        ACTION_OFFLINE
    };

    std::ostream &operator<<(std::ostream &stream, ActionType action_type);
    std::istream &operator>>(std::istream &stream, ActionType &action_type);

    enum DeviceType
    {
        DEVTYPE_UNKNOWN,
        DEVTYPE_TTY,
        DEVTYPE_DISK,
        DEVTYPE_PARTITION,
        DEVTYPE_LOOP
    };

    std::ostream &operator<<(std::ostream &stream, DeviceType device_type);
    std::istream &operator>>(std::istream &stream, DeviceType &device_type);

    //--------------------------------------------------------------------------
    // UDev DeviceInfo

    struct DeviceInfo : public core::types::Listable,
                        public core::types::enable_create_shared<DeviceInfo>
    {
        using This = DeviceInfo;
        using AttributeMap = std::map<std::string, std::string>;
        using AttributeList = std::vector<std::string>;
        using AttributeSet = std::set<std::string>;

    public:
        using ptr = std::shared_ptr<DeviceInfo>;

    protected:
        DeviceInfo(struct udev_device *device,
                   bool borrowed = false);

    public:
        virtual ~DeviceInfo();

        ActionType actionType() const noexcept;
        DeviceType deviceType() const noexcept;

        std::shared_ptr<DeviceInfo> parent_device() const;

        std::string action() const;
        std::string devpath() const;
        std::string subsystem() const;
        std::string devtype() const;
        std::string syspath() const;
        std::string sysname() const;
        std::string sysnum() const;
        std::string devnode() const;
        std::string driver() const;
        dev_t devnum() const;
        unsigned long long int seqnum() const;
        core::dt::Duration age() const;
        AttributeMap properties() const;
        AttributeList devlinks() const;
        AttributeSet tags() const;
        AttributeMap system_attributes() const;

    protected:
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;

    private:
        static AttributeMap attribute_map(
            struct udev_list_entry *list) noexcept;

        static AttributeList attribute_list(
            struct udev_list_entry *list) noexcept;

        static AttributeSet attribute_set(
            struct udev_list_entry *list) noexcept;

        static std::string string(
            const char *cstr) noexcept;

    public:
        udev_device *device;
        bool borrowed;
    };
}  // namespace cc::core::udev
