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
    // Event
    void encode(const core::status::Event &native, CC::Status::Event *idl) noexcept
    {
        idl->text(native.text());
        encode(native.domain(), &idl->domain());
        idl->origin(native.origin());
        idl->code(native.code());
        idl->symbol(native.symbol());
        encode(native.level(), &idl->level());
        encode(native.timepoint(), &idl->timestamp());
        encode(native.attributes(), &idl->attributes());
        idl->host(native.host());
    }

    void decode(const CC::Status::Event &idl, core::status::Event *native) noexcept
    {
        *native = decoded_event(idl);
    }

    core::status::Event decoded_event(const CC::Status::Event &idl) noexcept
    {
        return {
            idl.text(),                                           // text
            decoded<core::status::Domain>(idl.domain()),          // domain
            idl.origin(),                                         // origin
            static_cast<int>(idl.code()),                         // code
            idl.symbol(),                                         // symbol
            decoded<core::status::Level>(idl.level()),            // level
            decoded<core::dt::TimePoint>(idl.timestamp()),        // timepoint
            decoded<core::types::KeyValueMap>(idl.attributes()),  // attributes
            {},                                                   // contract_id
            idl.host(),                                           // host
        };
    }

    //==========================================================================
    // LogMessage

    void encode(const core::logging::Message &native,
                CC::Status::LogMessage *idl) noexcept
    {
        encode(static_cast<core::status::Event>(native),
               static_cast<CC::Status::Event *>(idl));
        idl->log_scope(native.scopename());
        idl->filename(native.path().string());
        idl->lineno(native.lineno());
        idl->function(native.function());
        idl->thread_id(native.thread_id());
    }

    core::logging::Message decoded_logmessage(CC::Status::LogMessage idl) noexcept
    {
        auto level = decoded<core::status::Level>(idl.level());

        core::logging::Scope::ptr scope =
            !idl.log_scope().empty()
                ? core::logging::Scope::create(idl.log_scope(), level)
                : log_scope;

        return {
            idl.text(),
            level,
            scope,
            decoded<core::dt::TimePoint>(idl.timestamp()),
            idl.filename(),
            idl.lineno(),
            idl.function(),
            static_cast<pid_t>(idl.thread_id()),
            idl.host(),
            idl.origin(),
            decoded<core::status::Domain>(idl.domain()),
            idl.code(),
            idl.symbol(),
            decoded<core::types::KeyValueMap>(idl.attributes()),
        };
    }

}  // namespace core::idl
