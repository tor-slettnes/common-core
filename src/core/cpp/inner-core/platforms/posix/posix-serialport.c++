// -*- c++ -*-
//==============================================================================
/// @file posix-serialport.c++
/// @brief Serial Port - POSIX implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-serialport.h++"
#include "status/exceptions.h++"

#include <unistd.h>
#include <fcntl.h>

namespace core::platform
{
    PosixSerialPortProvider::PosixSerialPortProvider(
        const std::string &implementation)
        : SerialPortProvider(implementation),
          fd_(-1),
          speed_(0)
    {
    }

    void PosixSerialPortProvider::open(const std::string &device, BaudRate speed)
    {
        if (!this->is_open())
        {
            this->fd_ = ::open(device.data(), O_RDWR);
            if (this->fd_ < 0)
            {
                throwf(core::exception::SystemError,
                       "open(%r)",
                       device);
            }

            this->device_ = device;

            if (tcgetattr(this->fd_, &this->tty_) != 0)
            {
                this->close();
                throw core::exception::SystemError("tcgetattr()");
            }

            this->apply_flags();
            this->apply_speed(speed);

            if (tcsetattr(this->fd_, TCSANOW, &this->tty_) != 0)
            {
                this->close();
                throw core::exception::SystemError("tcsetattr()");
            }
        }
    }

    void PosixSerialPortProvider::close()
    {
        if (this->is_open())
        {
            int result = ::close(this->fd_);
            if (result == -1)
            {
                throwf(core::exception::SystemError,
                       "close(%r)",
                       this->device_);
            }
            this->fd_ = -1;
            this->device_.clear();
            this->speed_ = 0;
        }
    }

    bool PosixSerialPortProvider::is_open() const
    {
        return (this->fd_ >= 0);
    }

    std::string PosixSerialPortProvider::device() const
    {
        return this->device_;
    }

    SerialPortProvider::BaudRate PosixSerialPortProvider::speed() const
    {
        return this->speed_;
    }

    void PosixSerialPortProvider::apply_flags()
    {
        // Clear parity bit
        this->tty_.c_cflag &= ~PARENB;
        // Clear stop field, use only one stop bit
        this->tty_.c_cflag &= ~CSTOPB;
        // Clear data size
        this->tty_.c_cflag &= ~CSIZE;
        // 8 data bits per byte
        this->tty_.c_cflag |= CS8;
        // Disable RTS/CTS flow control
        this->tty_.c_cflag &= ~CRTSCTS;
        // Turn on READ & ignore ctrl lines (CLOCAL = 1)
        this->tty_.c_cflag |= CREAD | CLOCAL;
        // Disable canonical (cooked) mode
        this->tty_.c_lflag &= ~ICANON;
        // Disable echo
        this->tty_.c_lflag &= ~ECHO;
        // Disable erasure
        this->tty_.c_lflag &= ~ECHOE;
        // Disable new-line echo
        this->tty_.c_lflag &= ~ECHONL;

        // Disable interpretation of INTR, QUIT and SUSP
        this->tty_.c_lflag &= ~ISIG;
        // Disable SW flow conrol
        this->tty_.c_iflag &= ~(IXON | IXOFF | IXANY);
        // Disable special handling of received bytes
        this->tty_.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
        // Prevent special interpretation of output bytes (e.g. newline chars)
        this->tty_.c_oflag &= ~OPOST;
        // Prevent conversion of newline to carriage return/line feed
        this->tty_.c_oflag &= ~ONLCR;
        // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
        // this->tty_.c_oflag &= ~OXTABS;
        // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)
        // this->tty_.c_oflag &= ~ONOEOT;

        // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
        this->tty_.c_cc[VTIME] = 10;
        this->tty_.c_cc[VMIN] = 0;
    }

    void PosixSerialPortProvider::apply_speed(BaudRate speed)
    {
        if (this->is_open())
        {
            cfsetspeed(&this->tty_, speed);
        }
    }

}  // namespace core::platform
