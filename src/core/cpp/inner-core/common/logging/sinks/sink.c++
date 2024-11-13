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
    Sink::Sink(const SinkID &sink_id)
        : is_open_(false),
          sink_id_(sink_id)
    {
    }

    Sink::~Sink()
    {
        // if (this->is_open_)
        // {
        //     this->close();
        // }
    }

    SinkID Sink::sink_id() const
    {
        return this->sink_id_;
    }

    void Sink::load_settings()
    {
        this->load_settings(core::settings
                                ->get(SETTING_LOG_SINKS)
                                .get(this->sink_id())
                                .as_kvmap());
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
