/// -*- c++ -*-
//==============================================================================
/// @file posix-providers.c++
/// @brief Superset of POSIX platform providers
/// @author Tor Slettnes
//==============================================================================

#include "posix-runtime.h++"
#include "posix-timezone.h++"
#include "posix-logsink.h++"
#include "posix-process.h++"
#include "posix-keycrypt.h++"
#include "posix-serialport.h++"
#include "posix-user.h++"

namespace cc::core::platform
{
    void register_posix_providers(const fs::path &exec_path)
    {
        runtime.registerProvider<PosixRunTimeProvider>();
        keycrypt.registerProvider<PosixKeyCryptProvider>();
        timezone.registerProvider<PosixTimeZoneProvider>();
        logsink.registerProvider<PosixLogSinkProvider>(exec_path);
        process.registerProvider<PosixProcessProvider>();
        serialport.registerProvider<PosixSerialPortProvider>();
        user.registerProvider<PosixUserProvider>();
    }

    void unregister_posix_providers()
    {
        user.unregisterProvider<PosixUserProvider>();
        serialport.unregisterProvider<PosixSerialPortProvider>();
        process.unregisterProvider<PosixProcessProvider>();
        logsink.unregisterProvider<PosixLogSinkProvider>();
        timezone.unregisterProvider<PosixTimeZoneProvider>();
        keycrypt.unregisterProvider<PosixKeyCryptProvider>();
        runtime.unregisterProvider<PosixRunTimeProvider>();
    }
}  // namespace cc::core::platform
