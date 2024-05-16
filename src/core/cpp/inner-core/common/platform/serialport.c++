// -*- c++ -*-
//==============================================================================
/// @file serialport.c++
/// @brief Serial Port - Abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "serialport.h++"

namespace cc::platform
{
    //==========================================================================
    /// @class SerialPort

    SerialPort::SerialPort(const std::string &device, BaudRate speed)
        : device_(device),
          speed_(speed)
    {
    }

    std::string SerialPort::device() const
    {
        return this->device_;
    }

    SerialPort::BaudRate SerialPort::speed() const
    {
        return this->speed_;
    }

    //==========================================================================
    /// Global instance, populated with the "best" provider for this system.

    ProviderProxy<SerialPortProvider> serialport("serialport");
}  // namespace cc::platform
