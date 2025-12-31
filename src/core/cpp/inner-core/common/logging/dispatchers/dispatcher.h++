/// -*- c++ -*-
//==============================================================================
/// @file dispatcher.h++
/// @brief Distribute a captured item to available sinks
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "logging/sinks/sink.h++"
#include "types/loggable.h++"
#include "types/valuemap.h++"

#include <memory>
#include <set>
#include <mutex>

namespace core::logging
{
    using SinkMap = types::ValueMap<SinkID, Sink::ptr>;

    class Dispatcher
    {
    public:
        using ptr = std::shared_ptr<Dispatcher>;

    public:
        void initialize();
        void deinitialize();

        void add_sinks(const SinkMap &sinks);

        Sink::ptr add_sink(const Sink::ptr &sink);
        Sink::ptr add_sink(const SinkID &sink_id,
                                   const Sink::ptr &sink);

        bool remove_sink(const SinkID &sink_id);
        bool remove_sink(const Sink::ptr &sink);

        Sink::ptr get_sink(const SinkID &sink_id) const;
        const SinkMap &sinks() const;

        bool is_applicable(const types::Loggable &item) const;
        virtual void submit(const types::Loggable::ptr &item);

    protected:
        SinkMap sinks_;

    private:
        std::mutex mtx_;
    };

    extern Dispatcher dispatcher;
}  // namespace core::logging
