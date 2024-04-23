// -*- c++ -*-
//==============================================================================
/// @file serialport.h++
/// @brief Serial Port - Abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include <string>

namespace core::platform
{
    /// @brief Abstract serial port interface
    class SerialPortProvider : public Provider
    {
        using Super = Provider;

    public:
        using BaudRate = int;

    protected:
        /// @param[in] implementation
        ///      Implementation name, e.g. final class name
        SerialPortProvider(const std::string &implementation);

    public:
        virtual void open(const std::string &device, BaudRate speed) = 0;
        virtual void close() = 0;
        virtual bool is_open() const = 0;
        virtual std::string device() const = 0;
        virtual BaudRate speed() const = 0;
    };

    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<SerialPortProvider> serialport;

}  // namespace core::platform
