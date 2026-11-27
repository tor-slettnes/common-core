// -*- c++ -*-
//==============================================================================
/// @file udev-enumerator.c++
/// @brief Enumerate devices through UDev
/// @author Tor Slettnes
//==============================================================================

#include "udev-monitor.h++"

namespace cc::core::udev
{
    Enumerator::Enumerator(const std::string &device_type)
        : udev(udev_new()),
          device_type(device_type)
    {
    }

    Enumerator::~Enumerator()
    {
        udev_unref(this->udev);
    }

    std::vector<std::shared_ptr<DeviceInfo>> Enumerator::enumerate() const
    {
        struct udev_enumerate *enumerator = udev_enumerate_new(this->udev);

        udev_enumerate_add_match_subsystem(
            enumerator,
            this->device_type.c_str());

        udev_enumerate_scan_devices(enumerator);

        std::vector<std::shared_ptr<DeviceInfo>> devices;

        struct udev_list_entry *list_entry = udev_enumerate_get_list_entry(
            enumerator);

        while (list_entry)
        {
            const char *path = udev_list_entry_get_name(list_entry);

            if (struct udev_device *device = udev_device_new_from_syspath(
                    this->udev,
                    path))
            {
                devices.emplace_back(DeviceInfo::create_shared(device));
            }

            list_entry = udev_list_entry_get_next(list_entry);
        }
        udev_enumerate_unref(enumerator);

        return devices;
    }
}  // namespace cc::core::udev
