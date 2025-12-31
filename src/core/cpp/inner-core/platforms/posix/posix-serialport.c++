// -*- c++ -*-
//==============================================================================
/// @file posix-serialport.c++
/// @brief Serial Port - POSIX implementation
/// @author Tor Slettnes
//==============================================================================

#include "posix-serialport.h++"
#include "status/exceptions.h++"

#include <unistd.h>
#include <fcntl.h>

namespace core::platform
{
    PosixSerialPort::PosixSerialPort(
        const std::string &device,
        BaudRate speed)
        : Super(device, speed),
          fd_(-1)
    {
    }

    void PosixSerialPort::open()
    {
        if (!this->is_open())
        {
            this->fd_ = ::open(this->device().data(), O_RDWR);
            if (this->fd_ < 0)
            {
                throwf(core::exception::SystemError,
                       "opening serial device %r",
                       this->device());
            }

            if (tcgetattr(this->fd_, &this->tty_) != 0)
            {
                this->close();
                throwf(core::exception::SystemError,
                       "getting attributes from serial device %r",
                       this->device());
            }

            this->apply_flags();
            this->apply_speed();

            if (tcsetattr(this->fd_, TCSANOW, &this->tty_) != 0)
            {
                this->close();
                throwf(core::exception::SystemError,
                       "setting attributes on serial device %r",
                       this->device());
            }
        }
    }

    void PosixSerialPort::close()
    {
        if (this->is_open())
        {
            int result = ::close(this->fd_);
            if (result == -1)
            {
                throwf(core::exception::SystemError,
                       "closing serial device %r",
                       this->device());
            }
            this->fd_ = -1;
        }
    }

    bool PosixSerialPort::is_open() const
    {
        return (this->fd_ >= 0);
    }

    void PosixSerialPort::write(const std::string &text)
    {
        if (int fd = this->fd_; this->fd_ >= 0)
        {
            if (::write(fd, text.data(), text.size()) < 0)
            {
                throwf(core::exception::SystemError,
                       "Writing to %s",
                       this->device());
            }
        }
    }

    std::optional<char> PosixSerialPort::readchar()
    {
        if (int fd = this->fd_; this->fd_ >= 0)
        {
            char c;
            if (::read(fd, &c, 1))
            {
                return c;
            }
            else
            {
                return {};
            }
        }
        else
        {
            throwf(core::exception::FailedPrecondition,
                   "Serial device %r is not open",
                   this->device());
        }
    }

    std::string PosixSerialPort::readline()
    {
        if (int fd = this->fd_; this->fd_ >= 0)
        {
            std::string buf;
            char c;
            ssize_t nread;
            while ((nread = ::read(fd, &c, 1)) > 0)
            {
                if (buf.size() >= buf.capacity())
                {
                    buf.reserve(buf.capacity() + 256);
                }
                buf.push_back(c);
                if (c == '\n')
                {
                    break;
                }
            }
            if (nread < 0)
            {
                throwf(core::exception::SystemError,
                       "reading from serial device %r",
                       this->device());
            }
            return buf;
        }
        else
        {
            throwf(core::exception::FailedPrecondition,
                   "Serial device %r is not open",
                   this->device());
        }
    }

    void PosixSerialPort::apply_flags()
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

    void PosixSerialPort::apply_speed()
    {
        if (this->is_open())
        {
            cfsetspeed(&this->tty_, this->speed());
        }
    }

    //==========================================================================
    /// @class PosixSerialPortProvider

    PosixSerialPortProvider::PosixSerialPortProvider(
        const std::string &implementation)
        : SerialPortProvider(implementation)
    {
    }

    SerialPortProvider::SerialPortPtr PosixSerialPortProvider::serialport(
        const std::string &device,
        SerialPort::BaudRate speed)
    {
        return std::make_shared<PosixSerialPort>(device, speed);
    }

}  // namespace core::platform
