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

namespace cc::core::platform
{
    PosixSerialPort::PosixSerialPort(
        const std::string &device)
        : Super(device),
          fd_(-1)
    {
    }

    PosixSerialPort::~PosixSerialPort()
    {
        if (int fd = this->fd_; fd >= 0)
        {
            ::close(fd);
        }
    }

    int PosixSerialPort::open()
    {
        if (!this->is_open())
        {
            int fd = ::open(this->device().data(), O_RDWR);
            if (!this->is_open())
            {
                throwf(core::exception::SystemError,
                       "opening serial device %r",
                       this->device());
            }

            try
            {
                this->tty_ = this->load_attributes(fd);
            }
            catch (...)
            {
                ::close(fd);
                throw;
            }

            this->fd_ = fd;
        }

        return this->fd_;
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
            this->tty_.reset();
        }
    }

    bool PosixSerialPort::is_open() const
    {
        return (this->fd_ >= 0);
    }

    int PosixSerialPort::fd() const
    {
        return this->fd_;
    }

    SerialPort::Parameters PosixSerialPort::parameters() const
    {
        if (this->tty_)
        {
            // Parameters params;
            // params.speed = This::extract_speed(*this->tty_);
            // params.rts_cts = (this->tty_->c_cflag & CRTSCTS) != 0;
            // params.xon_xoff = (this->tty_->c_cflag & (IXON | IXOFF)) != 0;
            // params.data_bits = This::extract_data_bits(*this->tty_);
            // params.parity = This::extract_parity(*this->tty_);
            // params.stop_bits = (this->tty_->c_cflag & CSTOPB) ? 2 : 1;
            // return params;

            return {
                .speed = This::extract_speed(*this->tty_),
                .rts_cts = (this->tty_->c_cflag & CRTSCTS) != 0,
                .xon_xoff = (this->tty_->c_cflag & (IXON | IXOFF)) != 0,
                .data_bits = This::extract_data_bits(*this->tty_),
                .parity = This::extract_parity(*this->tty_),
                .stop_bits = (this->tty_->c_cflag & CSTOPB) ? 2u : 1u,
            };
        }
        else
        {
            throwf(core::exception::FailedPrecondition,
                   "Serial device %r must be open in order to read attributes",
                   this->device());
        }
    }

    void PosixSerialPort::set_parameters(const Parameters &parameters)
    {
        if (this->tty_)
        {
            this->apply_flags(parameters, this->tty_.get());
            this->apply_speed(parameters, this->tty_.get());
            this->save_attributes(this->fd_, *this->tty_);
        }
        else
        {
            throwf(core::exception::FailedPrecondition,
                   "Serial device %r must be open in order set attributes",
                   this->device());
        }
    }

    void PosixSerialPort::write(const std::string &text) const
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
        else
        {
            throwf(core::exception::FailedPrecondition,
                   "Serial device %r must be open in order to write",
                   this->device());
        }
    }

    std::optional<char> PosixSerialPort::readchar() const
    {
        if (int fd = this->fd_; this->fd_ >= 0)
        {
            char c;
            ssize_t size = ::read(fd, &c, 1);
            if (size > 0)
            {
                return c;
            }
            else if (size == 0)
            {
                return {};
            }
            else
            {
                throwf(core::exception::SystemError,
                       "Reading from %s",
                       this->device());
            }
        }
        else
        {
            throwf(core::exception::FailedPrecondition,
                   "Serial device %r must be open in order to read",
                   this->device());
        }
    }

    std::string PosixSerialPort::readline() const
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
                   "Serial device %r must be open in order to read",
                   this->device());
        }
    }

    SerialPort::Speed PosixSerialPort::extract_speed(const struct termios &tty)
    {
        BaudRate input_speed = ::cfgetispeed(&tty);
        BaudRate output_speed = ::cfgetospeed(&tty);
        if (input_speed == output_speed)
        {
            return input_speed;
        }
        else
        {
            return BaudRatePair(input_speed, output_speed);
        }
    }

    std::uint8_t PosixSerialPort::extract_data_bits(const struct termios &tty)
    {
        switch (tty.c_cflag & CSIZE)
        {
        case CS5:
            return 5;
        case CS6:
            return 6;
        case CS7:
            return 7;
        case CS8:
            return 8;
        default:
            return 0;
        };
    }

    SerialPort::Parity PosixSerialPort::extract_parity(const struct termios &tty)
    {
        return (tty.c_cflag & PARENB) == 0 ? Parity::NONE
             : (tty.c_cflag & PARODD) == 0  ? Parity::EVEN
                                            : Parity::ODD;
    }

    std::shared_ptr<struct termios> PosixSerialPort::load_attributes(int fd)
    {
        auto tty = std::make_shared<struct termios>();
        if (tcgetattr(fd, tty.get()) != 0)
        {
            throwf(core::exception::SystemError,
                   "getting attributes from serial device %r",
                   this->device());
        }
        return tty;
    }

    void PosixSerialPort::save_attributes(int fd, const struct termios &tty)
    {
        if (tcsetattr(fd, TCSANOW, &tty) != 0)
        {
            throwf(core::exception::SystemError,
                   "setting attributes on serial device %r",
                   this->device());
        }
    }

    void PosixSerialPort::apply_flags(
        const Parameters &parameters,
        struct termios *tty)
    {
        switch (parameters.parity)
        {
        case Parity::NONE:
            // Clear parity bit
            This::apply_with_mask(0, PARENB | PARODD, tty);
            break;

        case Parity::ODD:
            // Set parity bit and odd bit
            This::apply_with_mask(PARENB | PARODD, PARENB | PARODD, tty);
            break;

        case Parity::EVEN:
            // Set parity bit, clear odd bit
            This::apply_with_mask(PARENB, PARENB | PARODD, tty);
            break;
        }

        // Set stop bit field iff two stop bits are in use
        This::apply_boolean_mask(parameters.stop_bits == 2, CSTOPB, tty);

        // Set data size mask to acording to data bits
        switch (parameters.data_bits)
        {
        case 5:
            This::apply_with_mask(CS5, CSIZE, tty);
            break;

        case 6:
            This::apply_with_mask(CS6, CSIZE, tty);
            break;

        case 7:
            This::apply_with_mask(CS7, CSIZE, tty);
            break;

        case 8:
            This::apply_with_mask(CS8, CSIZE, tty);
            break;
        }

        // Set or unset RTS/CTS flow control
        This::apply_boolean_mask(parameters.rts_cts, CRTSCTS, tty);

        // Set or unset XON/XOFF flow control
        This::apply_with_mask(
            parameters.xon_xoff ? (IXON | IXOFF) : 0,
            IXON | IXOFF | IXANY,
            tty);

        // Turn on READ & ignore ctrl lines (CLOCAL = 1)
        tty->c_cflag |= CREAD | CLOCAL;

        // Disable canonical (cooked) mode
        tty->c_lflag &= ~ICANON;

        // Disable echo
        tty->c_lflag &= ~ECHO;

        // Disable erasure
        tty->c_lflag &= ~ECHOE;

        // Disable new-line echo
        tty->c_lflag &= ~ECHONL;

        // Disable interpretation of INTR, QUIT and SUSP
        tty->c_lflag &= ~ISIG;

        // Disable special handling of received bytes
        tty->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

        // Prevent special interpretation of output bytes (e.g. newline chars)
        tty->c_oflag &= ~OPOST;

        // Prevent conversion of newline to carriage return/line feed
        tty->c_oflag &= ~ONLCR;

        // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
        // tty->c_oflag &= ~OXTABS;
        // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)
        // tty->c_oflag &= ~ONOEOT;

        // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
        tty->c_cc[VTIME] = 10;
        tty->c_cc[VMIN] = 0;
    }

    void PosixSerialPort::apply_speed(
        const Parameters &parameters,
        struct termios *tty)
    {
        switch (parameters.speed.index())
        {
        case 1:
            cfsetspeed(tty, std::get<1>(parameters.speed));
            break;

        case 2:
            cfsetispeed(tty, std::get<2>(parameters.speed).first);
            cfsetospeed(tty, std::get<2>(parameters.speed).second);
            break;
        }
    }

    void PosixSerialPort::apply_with_mask(
        const tcflag_t value,
        const tcflag_t mask,
        struct termios *tty)
    {
        tty->c_cflag &= ~mask;
        tty->c_cflag |= value;
    }

    void PosixSerialPort::apply_boolean_mask(
        bool value,
        const tcflag_t mask,
        struct termios *tty)
    {
        if (value)
        {
            tty->c_cflag |= mask;
        }
        else
        {
            tty->c_cflag &= ~mask;
        }
    }

    //==========================================================================
    /// @class PosixSerialPortProvider

    PosixSerialPortProvider::PosixSerialPortProvider(
        const std::string &implementation)
        : SerialPortProvider(implementation)
    {
    }

    SerialPort::ptr PosixSerialPortProvider::serialport(
        const std::string &device)
    {
        return std::make_shared<PosixSerialPort>(device);
    }

}  // namespace cc::core::platform
