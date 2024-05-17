/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-eventtypes.c++
/// @brief Encode/decode routines for common IDL error/status types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "translate-idl-eventtypes.h++"
#include "translate-idl-inline.h++"
#include "chrono/date-time.h++"

namespace core::idl
{
    //==========================================================================
    // Domain
    void encode(const core::status::Domain &native, CC::Status::Domain *idl) noexcept
    {
        *idl = static_cast<CC::Status::Domain>(native);
    }

    void decode(const CC::Status::Domain &idl, core::status::Domain *native) noexcept
    {
        *native = static_cast<core::status::Domain>(idl);
    }

    //==========================================================================
    // Severity Level
    void encode(const core::status::Level &native, CC::Status::Level *idl) noexcept
    {
        *idl = static_cast<CC::Status::Level>(native);
    }

    void decode(const CC::Status::Level &idl, core::status::Level *native) noexcept
    {
        *native = static_cast<core::status::Level>(idl);
    }

    //==========================================================================
    // Execution flow
    void encode(const core::status::Flow &native, CC::Status::Flow *idl) noexcept
    {
        *idl = static_cast<CC::Status::Flow>(native);
    }

    void decode(const CC::Status::Flow &idl, core::status::Flow *native) noexcept
    {
        *native = static_cast<core::status::Flow>(idl);
    }

    //==========================================================================
    // Event
    void encode(const core::status::Event &native, CC::Status::Event *idl) noexcept
    {
        idl->text(native.text());
        encode(native.domain(), &idl->domain());
        idl->origin(native.origin());
        idl->code(native.code());
        idl->symbol(native.symbol());
        encode(native.level(), &idl->level());
        encode(native.flow(), &idl->flow());
        encode(native.timepoint(), &idl->timestamp());
        encode(native.attributes(), &idl->attributes());
    }

    void decode(const CC::Status::Event &idl, core::status::Event *native) noexcept
    {
        *native = decoded_event(idl);
    }

    core::status::Event decoded_event(const CC::Status::Event &idl) noexcept
    {
        return {
            idl.text(),                                        // text
            decoded<core::status::Domain>(idl.domain()),         // domain
            idl.origin(),                                      // origin
            static_cast<int>(idl.code()),                      // code
            idl.symbol(),                                      // symbol
            decoded<core::status::Level>(idl.level()),           // level
            decoded<core::status::Flow>(idl.flow()),             // flow
            decoded<core::dt::TimePoint>(idl.timestamp()),       // timepoint
            decoded<core::types::KeyValueMap>(idl.attributes())  // attributes
        };
    }

    //==========================================================================
    // LogMessage

    void encode(const core::logging::Message &native,
                CC::Status::LogMessage *idl) noexcept
    {
        idl->text(native.text());
        idl->domain(CC::Status::Domain::APPLICATION);
        idl->origin(native.origin());
        idl->symbol(native.symbol());
        idl->code(native.code());
        encode(native.level(), &idl->level());
        encode(native.flow(), &idl->flow());
        encode(native.timepoint(), &idl->timestamp());
        encode(native.attributes(), &idl->attributes());
        idl->log_scope(native.scopename());
        idl->filename(native.path().string());
        idl->lineno(native.lineno());
        idl->function(native.function());
        idl->thread_id(native.thread_id());
    }

    core::logging::Message decoded_logmessage(CC::Status::LogMessage idl) noexcept
    {
        return {
            idl.text(),
            core::logging::scopes.get(idl.log_scope()),
            decoded<core::status::Level>(idl.level()),
            decoded<core::status::Flow>(idl.flow()),
            decoded<core::dt::TimePoint>(idl.timestamp()),
            idl.filename(),
            idl.lineno(),
            idl.function(),
            static_cast<pid_t>(idl.thread_id()),
            idl.origin(),
            idl.code(),
            idl.symbol(),
            decoded<core::types::KeyValueMap>(idl.attributes()),
        };
    }

}  // namespace core::idl
