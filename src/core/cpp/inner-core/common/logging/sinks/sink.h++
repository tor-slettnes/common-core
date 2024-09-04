/// -*- c++ -*-
//==============================================================================
/// @file sink.h++
/// @brief Data capture sink - abstract backend
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/loggable.h++"

#include <memory>

namespace core::logging
{

    //==========================================================================
    /// \class Sink
    /// \brief Abstract data receiver

    class Sink
    {
    public:
        using ptr = std::shared_ptr<Sink>;

    protected:
        Sink() {}

    public:
        virtual inline void open() {}
        virtual inline void close() {}

        virtual inline bool is_applicable(const types::Loggable &) const { return true; }
        virtual bool capture(const types::Loggable::ptr &loggable) = 0;
    };
}  // namespace core::logging
