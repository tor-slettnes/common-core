// -*- c++ -*-
//==============================================================================
/// @file udev-enumerator.h++
/// @brief Enumerate devices through UDev
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "udev-deviceinfo.h++"

#include <libudev.h>

namespace cc::core::udev
{
    class Enumerator
    {
    public:
        Enumerator(const std::string &device_type);
        virtual ~Enumerator();

    public:
        std::vector<std::shared_ptr<DeviceInfo>> enumerate() const;

    protected:
        struct udev *udev;
        std::string device_type;
    };
}  // namespace cc::core::udev
