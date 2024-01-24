/// -*- c++ -*-
//==============================================================================
/// @file sink.h++
/// @brief Data capture sink - abstract backend
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/loggable.h++"

#include <memory>

namespace cc::logging
{

    //==========================================================================
    /// \class Sink
    /// \brief Abstract data receiver

    class Sink
    {
    public:
        using Ref = std::shared_ptr<Sink>;

    protected:
        Sink() {}

    public:
        virtual inline void open() {}
        virtual inline void close() {}

        virtual inline bool is_applicable(const types::Loggable &) const { return true; }
        virtual bool capture(const types::Loggable::Ref &loggable) = 0;
    };
}  // namespace cc::logging
