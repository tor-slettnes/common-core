/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-eventtypes.c++
/// @brief Encode/decode routines for common IDL error/status types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "translate-idl-eventtypes.h++"
#include "translate-idl-inline.h++"
#include "chrono/date-time.h++"

namespace cc::idl
{
    //==========================================================================
    // Domain
    void encode(const cc::status::Domain &native, CC::Status::Domain *idl) noexcept
    {
        *idl = static_cast<CC::Status::Domain>(native);
    }

    void decode(const CC::Status::Domain &idl, cc::status::Domain *native) noexcept
    {
        *native = static_cast<cc::status::Domain>(idl);
    }

    //==========================================================================
    // Severity Level
    void encode(const cc::status::Level &native, CC::Status::Level *idl) noexcept
    {
        *idl = static_cast<CC::Status::Level>(native);
    }

    void decode(const CC::Status::Level &idl, cc::status::Level *native) noexcept
    {
        *native = static_cast<cc::status::Level>(idl);
    }

    //==========================================================================
    // Execution flow
    void encode(const cc::status::Flow &native, CC::Status::Flow *idl) noexcept
    {
        *idl = static_cast<CC::Status::Flow>(native);
    }

    void decode(const CC::Status::Flow &idl, cc::status::Flow *native) noexcept
    {
        *native = static_cast<cc::status::Flow>(idl);
    }

    //==========================================================================
    // Event
    void encode(const cc::status::Event &native, CC::Status::Event *idl) noexcept
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

    void decode(const CC::Status::Event &idl, cc::status::Event *native) noexcept
    {
        *native = decoded_event(idl);
    }

    cc::status::Event decoded_event(const CC::Status::Event &idl) noexcept
    {
        return {
            idl.text(),                                        // text
            decoded<cc::status::Domain>(idl.domain()),         // domain
            idl.origin(),                                      // origin
            static_cast<int>(idl.code()),                      // code
            idl.symbol(),                                      // symbol
            decoded<cc::status::Level>(idl.level()),           // level
            decoded<cc::status::Flow>(idl.flow()),             // flow
            decoded<cc::dt::TimePoint>(idl.timestamp()),       // timepoint
            decoded<cc::types::KeyValueMap>(idl.attributes())  // attributes
        };
    }

    //==========================================================================
    // LogMessage

    void encode(const cc::logging::Message &native,
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

    cc::logging::Message decoded_logmessage(CC::Status::LogMessage idl) noexcept
    {
        return {
            idl.text(),
            cc::logging::scopes.get(idl.log_scope()),
            decoded<cc::status::Level>(idl.level()),
            decoded<cc::status::Flow>(idl.flow()),
            decoded<cc::dt::TimePoint>(idl.timestamp()),
            idl.filename(),
            idl.lineno(),
            idl.function(),
            static_cast<pid_t>(idl.thread_id()),
            idl.origin(),
            idl.code(),
            idl.symbol(),
            decoded<cc::types::KeyValueMap>(idl.attributes()),
        };
    }

}  // namespace cc::idl
