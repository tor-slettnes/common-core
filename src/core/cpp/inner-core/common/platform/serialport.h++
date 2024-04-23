// -*- c++ -*-
//==============================================================================
/// @file serialport.h++
/// @brief Serial Port - Abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include <string>
#include <memory>

namespace core::platform
{
    //==========================================================================
    /// @class SerialPort
    /// @brief Abstract serial port interface
    class SerialPort
    {
    public:
        using BaudRate = int;

    protected:
        SerialPort(const std::string &device, BaudRate speed);

    public:
        std::string device() const;
        BaudRate speed() const;

        virtual void open() = 0;
        virtual void close() = 0;
        virtual bool is_open() const = 0;

        virtual void write(const std::string &text) = 0;
        virtual std::optional<char> readchar() = 0;
        virtual std::string readline() = 0;

    private:
        std::string device_;
        BaudRate speed_;
    };


    //==========================================================================
    /// @class SerialPortProvider
    /// @brief Abstract serial port provider

    class SerialPortProvider : public Provider
    {
        using Super = Provider;

    public:
        using SerialPortPtr = std::shared_ptr<SerialPort>;

    protected:
        using Provider::Provider;

    public:
        virtual SerialPortPtr serialport(
            const std::string &device,
            SerialPort::BaudRate speed) = 0;
    };

    //==========================================================================
    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<SerialPortProvider> serialport;

}  // namespace core::platform
