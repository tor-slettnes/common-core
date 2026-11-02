// -*- c++ -*-
//==============================================================================
/// @file serialport.h++
/// @brief Serial Port - Abstract interface
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "provider.h++"
#include <string>
#include <memory>
#include <variant>

namespace cc::core::platform
{
    //==========================================================================
    /// @class SerialPort
    /// @brief Abstract serial port interface
    class SerialPort
    {
    public:
        using ptr = std::shared_ptr<SerialPort>;

        using BaudRate = std::uint32_t;
        using BaudRatePair = std::pair<BaudRate, BaudRate>;

        using Speed = std::variant<
            std::monostate,  // no baudrate specified
            BaudRate,        // uniform input and output speed
            BaudRatePair>;   // separate input and output speeds

        enum class Parity
        {
            NONE,  // No parity
            ODD,   // Odd parity
            EVEN,  // Even parity
        };

        struct Parameters
        {
            Speed speed;                   // e.g. 115200 or {115200, 115200}
            bool rts_cts = false;          // harware flow control
            bool xon_xoff = false;         // software flow control
            uint data_bits = 8;            // character size
            Parity parity = Parity::NONE;  // parity bit
            uint stop_bits = 1;            // stop bits
        };

    protected:
        SerialPort(
            const std::string &device);

    public:
        std::string device() const;

        // Open the device. No serial port parameters are set.
        // Returns file desriptor to the serial device.
        virtual int open() = 0;

        // Close the device node.
        virtual void close() = 0;

        // Indicate whether the device is currently opened
        virtual bool is_open() const = 0;

        // Returns the file descriptor to the serial device,
        // or -1 if the device is not open.
        virtual int fd() const = 0;

        // Return serial port speed.
        // If unknown, return 0.
        // If input and output speed are different, return the higher value.
        BaudRate baud_rate() const;

        // Return current serial port parameters.
        // Device must be opened first.
        virtual Parameters parameters() const = 0;

        // Upate serial port parameters.
        // Device must be opened first.
        virtual void set_parameters(const Parameters &parameters) = 0;

        // Send a text string to the serial port.
        virtual void write(const std::string &text) const = 0;

        // read a single character from the serial port.
        virtual std::optional<char> readchar() const = 0;

        // read one line of text from the serial port.
        virtual std::string readline() const = 0;

    private:
        std::string device_;
    };

    //==========================================================================
    /// @class SerialPortProvider
    /// @brief Abstract serial port provider

    class SerialPortProvider : public Provider
    {
        using Super = Provider;

    protected:
        using Provider::Provider;

    public:
        virtual SerialPort::ptr serialport(
            const std::string &device) = 0;
    };

    //==========================================================================
    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<SerialPortProvider> serialport;

}  // namespace cc::core::platform
