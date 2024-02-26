// -*- c++ -*-
//==============================================================================
/// @brief Human Interface Device - abstract platform provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "types/symbolmap.h++"
#include "types/bytevector.h++"
#include "types/filesystem.h++"
#include "chrono/date-time.h++"
#include "thread/signaltemplate.h++"

#include <tuple>

namespace core::platform
{
    //==========================================================================
    // HID Bus Type

    enum class BusType
    {
        UNKNOWN = 0,
        USB = 1,
        BLUETOOTH = 2,
        I2C = 3,
        SPI = 4
    };

    extern types::SymbolMap<BusType> BusTypeNames;
    std::ostream &operator<<(std::ostream &stream, BusType bustype);

    //==========================================================================
    // HID Device Info

    using HIDVendorID = std::uint32_t;
    using HIDProductID = std::uint32_t;
    using HIDSerialNumber = std::string;
    using HIDDeviceKey = HIDSerialNumber;

    struct HIDDeviceInfo
    {
        fs::path path;
        HIDVendorID vendor_id;
        HIDProductID product_id;
        HIDSerialNumber serial_number;
        std::uint16_t release_number;
        std::string manufacturer;
        std::string product;
        std::uint16_t usage_page;  // Windows/Mac/hidraw only
        std::uint16_t usage;       // Windows/Mac/hidraw only
        int interface_number;
        BusType bus_type;
    };

    std::ostream &operator<<(std::ostream &stream, const HIDDeviceInfo &device_info);
    bool operator==(const HIDDeviceInfo &lhs, const HIDDeviceInfo &rhs);
    bool operator!=(const HIDDeviceInfo &lhs, const HIDDeviceInfo &rhs);

    //==========================================================================
    // HID Device

    class HIDDevice
    {
    public:
        virtual void write(const types::ByteVector &buffer) = 0;
        virtual types::ByteVector read() = 0;
        virtual std::optional<types::ByteVector> read(const dt::Duration &timeout) = 0;
        virtual void send_feature_report(const types::ByteVector &data) = 0;
        virtual types::ByteVector get_feature_report(std::uint8_t report_id = 0) = 0;
        virtual types::ByteVector get_input_report(std::uint8_t report_id = 0) = 0;
        virtual std::string get_manufacturer() = 0;
        virtual std::string get_product() = 0;
        virtual std::string get_serial_number() = 0;
        virtual HIDDeviceInfo get_info() = 0;
        virtual std::string get_indexed_string(int index) = 0;
        // virtual types::ByteVector get_report_descriptor() = 0;
        virtual std::string hid_api_version() = 0;
    };

    using HIDDeviceRef = std::shared_ptr<HIDDevice>;

    //==========================================================================
    // HID Provider

    class HIDProvider : public Provider
    {
        using This = HIDProvider;
        using Super = Provider;

    public:
        using Provider::Provider;

        void initialize() override;
        void deinitialize() override;

        virtual std::list<HIDDeviceInfo> enumerate(
            uint vendor_id = 0,
            uint product_id = 0) = 0;

        virtual HIDDeviceRef open(uint vendor_id,
                                  uint product_id) = 0;

        virtual HIDDeviceRef open(uint vendor_id,
                                  uint product_id,
                                  const std::string &serial_number) = 0;

        virtual HIDDeviceRef open(const fs::path &path) = 0;

    public:
        void start_monitoring(const dt::Duration &poll_interval = std::chrono::seconds(1));
        void stop_monitoring();
        void monitor_worker();
    };

    //==========================================================================
    // Signals

    extern signal::MappingSignal<HIDDeviceInfo> signal_hid_device;

    //==========================================================================
    // Global instance

    extern ProviderProxy<HIDProvider> hid;

}  // namespace core::platform
