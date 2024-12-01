/// -*- c++ -*-
//==============================================================================
/// @file sink.c++
/// @brief Data capture sink - abstract backend
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sink.h++"
#include "settings/settings.h++"

#include <iostream>

namespace core::logging
{
    //--------------------------------------------------------------------------
    /// @class Sink

    Sink::Sink(const SinkID &sink_id,
               const SinkType &sink_type)
        : is_open_(false),
          sink_id_(sink_id),
          sink_type_(sink_type)
    {
    }

    SinkID Sink::sink_id() const
    {
        return this->sink_id_;
    }

    SinkType Sink::sink_type() const
    {
        return this->sink_type_;
    }

    void Sink::set_sink_type(const SinkType &sink_type)
    {
        this->sink_type_ = sink_type;
    }

    bool Sink::is_applicable(const types::Loggable &) const
    {
        return true;
    }

    bool Sink::is_open() const
    {
        return this->is_open_;
    }

    void Sink::open()
    {
        this->is_open_ = true;
    }

    void Sink::close()
    {
        this->is_open_ = false;
    }
}  // namespace core::logging
