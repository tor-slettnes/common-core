// -*- c++ -*-
//==============================================================================
/// @brief Human Interface Device - abstract platform provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "hid.h++"
#include "types/partslist.h++"
#include "chrono/scheduler.h++"

#include <map>

namespace cc::platform
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

    std::ostream &operator<<(std::ostream &stream, const HIDDeviceInfo &info)
    {
        types::PartsList list;
        list.add("path", info.path.string(), !info.path.empty());
        list.add("vendor_id", info.vendor_id, true, "%04x");
        list.add("product_id", info.product_id, true, "%04x");
        list.add("serial_number", info.serial_number, !info.serial_number.empty());
        list.add("release_number", info.release_number, info.release_number != 0);
        list.add("manufacturer", info.manufacturer, !info.manufacturer.empty());
        list.add("product", info.product, !info.product.empty());
        list.add("usage_page", info.usage_page, info.usage_page != 0);
        list.add("usage", info.usage, info.usage != 0);
        list.add("interface_number", info.interface_number);
        list.add("bus_type", info.bus_type, info.bus_type != BusType::UNKNOWN);
        stream << list;
        return stream;
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
        cc::scheduler.add_if_missing(
            this->name,
            [&]() {  // invocation
                this->monitor_worker();
            },
            poll_interval,  // interval
            cc::Scheduler::ALIGN_START);
    }

    void HIDProvider::stop_monitoring()
    {
        cc::scheduler.remove(this->name);
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

    signal::MappedSignal<HIDDeviceInfo> signal_hid_device("hid_device", true);

    //==========================================================================
    // Global instance

    ProviderProxy<HIDProvider> hid("HID");

}  // namespace cc::platform
