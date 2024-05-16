/// -*- c++ -*-
//==============================================================================
/// @file posix-providers.c++
/// @brief Superset of POSIX platform providers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-runtime.h++"
#include "posix-timezone.h++"
#include "posix-logsink.h++"
#include "posix-process.h++"
#include "posix-serialport.h++"

namespace core::platform
{
    void register_posix_providers(const std::string &exec_name)
    {
        runtime.registerProvider<PosixRunTimeProvider>();
        timezone.registerProvider<PosixTimeZoneProvider>();
        logsink.registerProvider<PosixLogSinkProvider>(exec_name);
        process.registerProvider<PosixProcessProvider>();
        serialport.registerProvider<PosixSerialPortProvider>();
    }

    void unregister_posix_providers()
    {
        serialport.unregisterProvider<PosixSerialPortProvider>();
        process.unregisterProvider<PosixProcessProvider>();
        logsink.unregisterProvider<PosixLogSinkProvider>();
        timezone.unregisterProvider<PosixTimeZoneProvider>();
        runtime.unregisterProvider<PosixRunTimeProvider>();
    }
}  // namespace core::platform
