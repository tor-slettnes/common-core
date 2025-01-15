/// -*- c++ -*-
//==============================================================================
/// @file dispatcher.h++
/// @brief Distribute a captured item to available sinks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logging/sinks/sink.h++"
#include "logging/sinks/factory.h++"
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
        Sink::ptr add_sink(const Sink::ptr &sink);
        virtual Sink::ptr add_sink(const SinkID &sink_id,
                                   const Sink::ptr &sink);

        Sink::ptr emplace_sink(const SinkID &sink_id,
                               SinkFactory *factory,
                               const types::KeyValueMap &settings,
                               status::Level threshold);

        virtual bool remove_sink(const SinkID &sink_id);
        virtual bool remove_sink(const Sink::ptr &sink);

        Sink::ptr get_sink(const SinkID &sink_id) const;
        const SinkMap &sinks() const;

        virtual void initialize();
        virtual void deinitialize();

        virtual bool is_applicable(const types::Loggable &item) const;
        virtual void submit(const types::Loggable::ptr &item);

    protected:
        SinkMap sinks_;

    private:
        std::mutex mtx_;
    };

    extern Dispatcher dispatcher;
}  // namespace core::logging
