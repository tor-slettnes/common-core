// -*- c++ -*-
//==============================================================================
/// @brief Human Interface Device - abstract platform provider
/// @author Tor Slettnes
//==============================================================================

#include "hid.h++"
#include "types/partslist.h++"
#include "chrono/scheduler.h++"

#include <map>

namespace core::platform
{
    //==========================================================================
    // Bus Type

    types::SymbolMap<BusType> BusTypeNames = {
        {BusType::USB, "USB"},
        {BusType::BLUETOOTH, "BLUETOOTH"},
        {BusType::I2C, "I2C"},
        {BusType::SPI, "SPI"},
    };

    std::ostream &operator<<(std::ostream &stream, BusType bustype)
    {
        return BusTypeNames.to_stream(stream, bustype, "UNKNOWN");
    }

    //==========================================================================
    // Device Info

    types::TaggedValueList &operator<<(types::TaggedValueList &tvlist,
                                       const HIDDeviceInfo &info)
    {
        tvlist.append_if(!info.path.empty(), "path", info.path.string());
        tvlist.append("vendor_id", info.vendor_id);
        tvlist.append("product_id", info.product_id);
        tvlist.append_if(!info.serial_number.empty(), "serial_number", info.serial_number);
        tvlist.append_if(info.release_number != 0, "release_number", info.release_number);
        tvlist.append_if(!info.manufacturer.empty(), "manufacturer", info.manufacturer);
        tvlist.append_if(!info.product.empty(), "product", info.product);
        tvlist.append_if(info.usage_page != 0, "usage_page", info.usage_page);
        tvlist.append_if(info.usage != 0, "usage", info.usage);
        tvlist.append("interface_number", info.interface_number);
        tvlist.append_if(info.bus_type != BusType::UNKNOWN, "bus_type", info.bus_type);
    }

    std::ostream &operator<<(std::ostream &stream, const HIDDeviceInfo &info)
    {
        return stream << types::TaggedValueList::create_from(info);
    }

    bool operator==(const HIDDeviceInfo &lhs, const HIDDeviceInfo &rhs)
    {
        return (lhs.path == rhs.path) &&
               (lhs.vendor_id == rhs.vendor_id) &&
               (lhs.product_id == rhs.product_id) &&
               (lhs.serial_number == rhs.serial_number) &&
               (lhs.release_number == rhs.release_number) &&
               (lhs.manufacturer == rhs.manufacturer) &&
               (lhs.product == rhs.product) &&
               (lhs.interface_number == rhs.interface_number) &&
               (lhs.bus_type == rhs.bus_type);
    }

    bool operator!=(const HIDDeviceInfo &lhs, const HIDDeviceInfo &rhs)
    {
        return !(lhs == rhs);
    }

    //==========================================================================
    // HID Provider

    void HIDProvider::initialize()
    {
        Super::initialize();
        // this->start_monitoring();
    }

    void HIDProvider::deinitialize()
    {
        // this->stop_monitoring();
        Super::deinitialize();
    }

    void HIDProvider::start_monitoring(const dt::Duration &poll_interval)
    {
        core::scheduler.add_if_missing(
            this->name,
            [&]() {  // invocation
                this->monitor_worker();
            },
            poll_interval,  // interval
            core::Scheduler::ALIGN_START);
    }

    void HIDProvider::stop_monitoring()
    {
        core::scheduler.remove(this->name);
    }

    void HIDProvider::monitor_worker()
    {
        std::unordered_map<std::string, HIDDeviceInfo> available_devices;
        for (const HIDDeviceInfo &info : this->enumerate())
        {
            available_devices.insert_or_assign(info.serial_number, info);
        }
        signal_hid_device.synchronize(available_devices);
    }

    //==========================================================================
    // Signals

    signal::MappingSignal<HIDDeviceInfo> signal_hid_device("hid_device", true);

    //==========================================================================
    // Global instance

    ProviderProxy<HIDProvider> hid("HID");

}  // namespace core::platform
