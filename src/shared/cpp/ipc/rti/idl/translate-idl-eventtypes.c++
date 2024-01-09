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
    // Severity
    void encode(const cc::status::Level &native, CC::Status::Level *idl) noexcept
    {
        *idl = static_cast<CC::Status::Level>(native);
    }

    void decode(const CC::Status::Level &idl, cc::status::Level *native) noexcept
    {
        *native = static_cast<cc::status::Level>(idl);
    }

    //==========================================================================
    // Event
    void encode(const cc::status::Event &native, CC::Status::Event *idl) noexcept
    {
        idl->text(native.text());
        encode(native.domain(), &idl->domain());
        idl->origin(native.origin());
        encode(native.level(), &idl->level());
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
            decoded<cc::status::Level>(idl.level()),           // level
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
        encode(native.level(), &idl->level());
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
            cc::logging::scopes.get(idl.log_scope()),
            decoded<cc::status::Level>(idl.level()),
            decoded<cc::dt::TimePoint>(idl.timestamp()),
            idl.filename(),
            idl.lineno(),
            idl.function(),
            static_cast<pid_t>(idl.thread_id()),
            idl.origin(),
            decoded<cc::types::KeyValueMap>(idl.attributes()),
            idl.text(),
        };
    }

    //==========================================================================
    // ErrorEvent
    void encode(const cc::status::Error &native, CC::Status::ErrorEvent *idl) noexcept
    {
        encode(static_cast<cc::status::Event>(native), static_cast<CC::Status::Event *>(idl));
        idl->code(native.code());
        idl->symbol(native.symbol());
    }

    void decode(const CC::Status::ErrorEvent &idl, cc::status::Error *native) noexcept
    {
        *native = decoded_error(idl);
    }

    cc::status::Error decoded_error(const CC::Status::ErrorEvent &idl) noexcept
    {
        return {
            idl.text(),                                        // text
            decoded<cc::status::Domain>(idl.domain()),         // domain
            idl.origin(),                                      // origin
            static_cast<int>(idl.code()),                      // code
            idl.symbol(),                                      // symbol
            decoded<cc::status::Level>(idl.level()),           // level
            decoded<cc::dt::TimePoint>(idl.timestamp()),       // timepoint
            decoded<cc::types::KeyValueMap>(idl.attributes())  // attributes
        };
    }

}  // namespace cc::idl
