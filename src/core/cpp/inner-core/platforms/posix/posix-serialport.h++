// -*- c++ -*-
//==============================================================================
/// @file posix-serialport.h++
/// @brief Serial Port - POSIX implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/serialport.h++"

#include <termios.h>

namespace core::platform
{
    /// @brief Seral port interface for POSIX systems
    class PosixSerialPortProvider : public SerialPortProvider
    {
        using This = PosixSerialPortProvider;
        using Super = SerialPortProvider;

    protected:
        PosixSerialPortProvider(const std::string &implementation = "PosixSerialPortProvider");

    public:
        void open(const std::string &device, BaudRate speed) override;
        void close() override;
        bool is_open() const override;
        std::string device() const override;
        BaudRate speed() const override;

    private:
        void apply_flags();
        void apply_speed(BaudRate speed);

    private:
        struct termios tty_;
        int fd_;
        std::string device_;
        BaudRate speed_;
    };

}  // namespace core::platform
