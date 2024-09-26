// -*- c++ -*-
//==============================================================================
/// @file posix-serialport.h++
/// @brief Serial Port - POSIX implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/serialport.h++"
#include "types/create-shared.h++"

#include <termios.h>

namespace core::platform
{
    //==========================================================================
    /// @class PosixSerialPort
    /// @brief Seral port interface for POSIX systems

    class PosixSerialPort : public SerialPort,
                            public types::enable_create_shared<PosixSerialPort>
    {
        using This = PosixSerialPort;
        using Super = SerialPort;

    public:
        PosixSerialPort(
            const std::string &device,
            BaudRate speed);

        void open() override;
        void close() override;
        bool is_open() const override;

        void write(const std::string &text) override;
        std::optional<char> readchar() override;
        std::string readline() override;

    private:
        void apply_flags();
        void apply_speed();

    private:
        struct termios tty_;
        int fd_;
    };

    //==========================================================================
    /// @class PosixSerialPortProvider
    /// @brief Seral port provider for POSIX systems

    class PosixSerialPortProvider : public SerialPortProvider
    {
        using This = PosixSerialPortProvider;
        using Super = SerialPortProvider;

    protected:
        PosixSerialPortProvider(
            const std::string &implementation = "PosixSerialPortProvider");

    public:
        SerialPortPtr serialport(
            const std::string &device,
            SerialPort::BaudRate speed) override;
    };

}  // namespace core::platform
