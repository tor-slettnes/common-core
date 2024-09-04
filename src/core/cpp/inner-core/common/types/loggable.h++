/// -*- c++ -*-
//==============================================================================
/// @file loggable.h++
/// @brief Abstract loggable item, base for telemetry, log entry, error, etc.
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <memory>

namespace core::types
{
    //==========================================================================
    /// \class Loggable
    /// \brief Abstract loggable item.
    ///
    /// Specializations include:
    ///   - `status::Event`, and in turn `logging:Message`
    ///   - `status::Telemetry`
    ///   - `types::BinaryData`

    class Loggable
    {
    public:
        using ptr = std::shared_ptr<Loggable>;

    public:
        virtual ~Loggable() {}  // Hack to ensure class is polymorphic
    };

}  // namespace core::types
