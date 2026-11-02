// -*- c++ -*-
//==============================================================================
/// @file posix-serialport.h++
/// @brief Serial Port - POSIX implementation
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "platform/serialport.h++"
#include "types/create-shared.h++"

#include <termios.h>

namespace cc::core::platform
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
            const std::string &device);

        ~PosixSerialPort();

        int open() override;
        void close() override;
        bool is_open() const override;
        int fd() const override;

        Parameters parameters() const override;
        void set_parameters(const Parameters &parameters) override;

        void write(const std::string &text) const override;
        std::optional<char> readchar() const override;
        std::string readline() const override;

    public:
        static Speed extract_speed(const struct termios &tty);
        static std::uint8_t extract_data_bits(const struct termios &tty);
        static Parity extract_parity(const struct termios &tty);

    private:
        std::shared_ptr<struct termios> load_attributes(int fd);
        void save_attributes(int fd, const struct termios &tty);

        static void apply_flags(
            const Parameters &parameters,
            struct termios *tty);

        static void apply_speed(
            const Parameters &parameters,
            struct termios *tty);

        static void apply_with_mask(
            const tcflag_t value,
            const tcflag_t mask,
            struct termios *tty);

        static void apply_boolean_mask(
            bool value,
            const tcflag_t mask,
            struct termios *tty);

    private:
        int fd_;
        std::shared_ptr<struct termios> tty_;
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
        SerialPort::ptr serialport(
            const std::string &device) override;
    };

}  // namespace cc::core::platform
