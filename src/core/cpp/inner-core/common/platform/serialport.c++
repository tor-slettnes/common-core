// -*- c++ -*-
//==============================================================================
/// @file serialport.c++
/// @brief Serial Port - Abstract interface
/// @author Tor Slettnes
//==============================================================================

#include "serialport.h++"
#include <algorithm>

namespace cc::core::platform
{
    //==========================================================================
    /// @class SerialPort

    SerialPort::SerialPort(const std::string &device)
        : device_(device)
    {
    }

    std::string SerialPort::device() const
    {
        return this->device_;
    }

    SerialPort::BaudRate SerialPort::baud_rate() const
    {
        const Speed &variant = this->parameters().speed;
        switch (variant.index())
        {
        case 1:
            return std::get<1>(variant);

        case 2:
            return std::max(std::get<2>(variant).first,
                            std::get<2>(variant).second);
        default:
            return 0;
        }
    }


    //==========================================================================
    /// Global instance, populated with the "best" provider for this system.

    ProviderProxy<SerialPortProvider> serialport("serialport");
}  // namespace cc::core::platform
