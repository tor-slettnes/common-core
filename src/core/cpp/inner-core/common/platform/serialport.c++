// -*- c++ -*-
//==============================================================================
/// @file serialport.c++
/// @brief Serial Port - Abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "serialport.h++"

namespace core::platform
{
    SerialPortProvider::SerialPortProvider(const std::string &implementation)
        : Provider(implementation)
    {
    }

    ProviderProxy<SerialPortProvider> serialport("serialport");
}  // namespace core::platform
