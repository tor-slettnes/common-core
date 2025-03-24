// -*- c++ -*-
//==============================================================================
/// @file vfs-linux-volmon.h++
/// @brief VFS service - Monitor filesystem evens via Linux udev
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"
#include "types/listable.h++"
#include "thread/signaltemplate.h++"

#include <libudev.h>

#include <string>
#include <map>
#include <vector>
#include <set>
#include <thread>

#define UDEV_DEVTYPE    "block"
#define BLOCK_SIZE      512
#define PROPERTY_ACTION "ACTION"

namespace vfs::local::volume
{
    enum ActionType
    {
        ACTION_UNKNOWN,
        ACTION_ADD,
        ACTION_REMOVE,
        ACTION_CHANGE,
        ACTION_ONLINE,
        ACTION_OFFLINE
    };

    enum DeviceType
    {
        DEVTYPE_UNKNOWN,
        DEVTYPE_DISK,
        DEVTYPE_PARTITION,
        DEVTYPE_LOOP
    };

    enum ParameterType
    {
        PARAM_DRIVERS,
        PARAM_SUBSYSTEM,
        PARAM_DEVTYPE,
        PARAM_SYSPATH,
        PARAM_SYSNAME,
        PARAM_SYSNUM,
        PARAM_DEVNODE
    };

    using DeviceLookupMethod = const char *(*)(udev_device *);

    struct Event : public core::types::Listable
    {
        using KeyValueMap = core::types::KeyValueMap;
        using ValueList = std::vector<std::string>;
        using ValueSet = std::set<std::string>;

    public:
        using ptr = std::shared_ptr<Event>;

    public:
        Event(struct udev_device *device,
              ActionType actionType = ACTION_UNKNOWN);

        ActionType actionType() const noexcept;
        DeviceType deviceType() const noexcept;

    protected:
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;

    private:
        static core::types::KeyValueMap valuemap(struct udev_list_entry *list) noexcept;
        static ValueList valuelist(struct udev_list_entry *list) noexcept;
        static ValueSet valueset(struct udev_list_entry *list) noexcept;
        static core::types::KeyValueMap systemattributes(struct udev_device *device) noexcept;
        static std::string string(const char *cstr) noexcept;
        static udev_device *find_parent(udev_device *device,
                                        ParameterType parameter,
                                        const std::string &value) noexcept;

    public:
        std::string action, devpath, subsystem, devtype, syspath,
            sysname, sysnum, devnode, driver;
        dev_t devnum;
        unsigned long long int seqnum;
        core::dt::Duration age;
        bool is_usb;

        core::types::KeyValueMap properties, sysattrs;
        ValueList devlinks;
        ValueSet tags;

    private:
        ActionType actionType_;
    };

    struct DiskInfo : public core::types::Listable
    {
        DiskInfo(const Event &event);
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;
        static bool is_removable(const Event &event);

        std::string devnode;
        std::string serial;
        bool removable;
        bool writable;
    };

    struct PartitionInfo : public core::types::Listable
    {
        PartitionInfo(const Event &event);
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;
        std::string friendlyName() const;

        std::string devnode;
        std::string sysname;
        std::string type;
        std::string serial;
        std::string label;
        std::string fstype;
        std::string uuid;
        std::string vendor;
        std::string model;
        uint partnumber;
        bool writable;
    };

    //==========================================================================
    /// BaseIterator - Base for Enumerator and Monitor, below

    class Base
    {
    public:
        Base();
        ~Base();
        virtual bool available() const;
        virtual void init(){};
        virtual Event::ptr next() = 0;

    protected:
        struct udev *udev = nullptr;
        struct udev_device *device = nullptr;
    };

    //==========================================================================
    /// Volume Enumerator - Iterate over devices in system

    class Enumerator : public Base
    {
    public:
        Enumerator();
        ~Enumerator();

        void init() override;
        Event::ptr next() override;

    protected:
        struct udev_enumerate *enumerator = nullptr;
        struct udev_list_entry *devices = nullptr, *dev_list_entry = nullptr;
    };

    //==========================================================================
    /// Volume Monitor - monitor additions/removal/changes in udev

    class Monitor : public Base
    {
    public:
        Monitor();
        ~Monitor();

        bool available() const override;
        void init() override;
        Event::ptr next() override;

        void start();
        void stop();

    public:
        core::signal::DataSignal<Event> signal_event;

    private:
        void run();

    protected:
        struct udev_monitor *monitor = nullptr;
        int fd = -1;
        bool running = false;

    private:
        std::thread runthread;
    };
}  // namespace vfs::local::volume
