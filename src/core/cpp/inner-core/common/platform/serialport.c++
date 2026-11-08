// -*- c++ -*-
//==============================================================================
/// @file serialport.c++
/// @brief Serial Port - Abstract interface
/// @author Tor Slettnes
//==============================================================================

#include "serialport.h++"
#include <algorithm>

namespace cc::core::platform
{
    const types::SymbolMap<SerialPort::Parity> parity_names = {
        {SerialPort::Parity::NONE, "NONE"},
        {SerialPort::Parity::ODD, "ODD"},
        {SerialPort::Parity::EVEN, "EVEN"},
    };

    //==========================================================================
    /// @class SerialPort::Parameters

    void SerialPort::Parameters::to_tvlist(types::TaggedValueList *tvlist) const
    {
        switch (this->speed.index())
        {
        case 1:
            tvlist->append("speed", std::get<1>(this->speed));
            break;

        case 2:
            tvlist->append("speed[in]", std::get<2>(this->speed).first);
            tvlist->append("speed[out]", std::get<2>(this->speed).second);
            break;
        }

        tvlist->extend({
            {"rts/cts", this->rts_cts},
            {"xon/xoff", this->xon_xoff},
            {"data bits", this->data_bits},
            {"parity", parity_names.to_string(this->parity)},
            {"stop bits", this->stop_bits},
        });
    }

    SerialPort::BaudRate SerialPort::Parameters::get_speed(int direction) const
    {
        switch (this->speed.index())
        {
        case 1:
            return std::get<1>(this->speed);

        case 2:
        {
            const auto &[in, out] = std::get<2>(this->speed);
            return (direction == 0) ? in
                 : (direction == 1) ? out
                                    : std::max(in, out);
        }

        default:
            return 0;
        }
    }

    //==========================================================================
    /// @class SerialPort

    SerialPort::SerialPort(const std::string &device)
        : device_(device)
    {
    }

    const std::string &SerialPort::device() const
    {
        return this->device_;
    }

    //==========================================================================
    /// Global instance, populated with the "best" provider for this system.

    ProviderProxy<SerialPortProvider> serialport("serialport");
}  // namespace cc::core::platform
