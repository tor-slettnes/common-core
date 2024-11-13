/// -*- c++ -*-
//==============================================================================
/// @file sink.h++
/// @brief Data capture sink - abstract backend
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/loggable.h++"
#include "types/value.h++"

#include <memory>
#include <string>

namespace core::logging
{
    using SinkID = std::string;

    //--------------------------------------------------------------------------
    // Constants

    constexpr auto SETTING_LOG_SINKS = "log sinks";


    //--------------------------------------------------------------------------
    /// \class Sink
    /// \brief Abstract data receiver

    class Sink
    {
    public:
        using ptr = std::shared_ptr<Sink>;

    protected:
        Sink(const SinkID &sink_id);
        virtual ~Sink();

    public:
        SinkID sink_id() const;
        void load_settings();

    protected:
        virtual void load_settings(const types::KeyValueMap &settings) {}

    public:
        virtual bool is_applicable(const types::Loggable &) const;
        virtual bool is_open() const;
        virtual void open();
        virtual void close();
        virtual bool capture(const types::Loggable::ptr &loggable) = 0;

    private:
        bool is_open_;
        SinkID sink_id_;
    };

}  // namespace core::logging
