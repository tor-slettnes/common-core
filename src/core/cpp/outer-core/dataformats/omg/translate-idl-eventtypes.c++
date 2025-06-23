/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-eventtypes.c++
/// @brief Encode/decode routines for common IDL error/status types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "translate-idl-eventtypes.h++"
#include "translate-idl-inline.h++"
#include "chrono/date-time.h++"

namespace idl
{
    //==========================================================================
    // Domain
    void encode(const core::status::Domain &native, CC::Status::Domain *idl) noexcept
    {
        *idl = encoded(native);
    }

    void decode(const CC::Status::Domain &idl, core::status::Domain *native) noexcept
    {
        *native = decoded_domain(idl);
    }

    CC::Status::Domain encoded(const core::status::Domain &native) noexcept
    {
        return static_cast<CC::Status::Domain>(native);
    }

    core::status::Domain decoded_domain(const CC::Status::Domain &idl) noexcept
    {
        return static_cast<core::status::Domain>(idl);
    }

    //==========================================================================
    // Severity Level
    void encode(const core::status::Level &native, CC::Status::Level *idl) noexcept
    {
        *idl = encoded(native);
    }

    void decode(const CC::Status::Level &idl, core::status::Level *native) noexcept
    {
        *native = decoded_level(idl);
    }

    CC::Status::Level encoded(const core::status::Level &native) noexcept
    {
        return static_cast<CC::Status::Level>(native);
    }

    core::status::Level decoded_level(const CC::Status::Level &idl) noexcept
    {
        return static_cast<core::status::Level>(idl);
    }

    //==========================================================================
    // Event

    void encode(const core::status::Event &native, CC::Status::Event *idl) noexcept
    {
        encode(native.timepoint(), &idl->timestamp());
        encode(native.attributes(), &idl->attributes());

        idl->text(native.text());
        idl->level(encoded(native.level()));
        idl->origin(native.origin());
    }

    //==========================================================================
    // Error

    void encode(const core::status::Error &native, CC::Status::Error *idl) noexcept
    {
        encode(native, static_cast<CC::Status::Event *>(idl));

        idl->domain(encoded(native.domain()));
        idl->code(native.code());
        idl->symbol(native.symbol());
    }

    void decode(const CC::Status::Error &idl, core::status::Error *native) noexcept
    {
        *native = decoded_error(idl);
    }

    core::status::Error decoded_error(const CC::Status::Error &idl) noexcept
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
        };
    }

    //==========================================================================
    // LogMessage

    void encode(const core::logging::Message &native,
                CC::Status::LogMessage *idl) noexcept
    {
        encode(native, static_cast<CC::Status::Event *>(idl));

        idl->host(native.host());
        idl->log_scope(native.scopename());
        idl->filename(native.path().string());
        idl->lineno(native.lineno());
        idl->function(native.function());
        idl->thread_id(native.thread_id());
        idl->thread_name(native.thread_name());
        idl->task_name(native.task_name());
    }

    void decode(const CC::Status::LogMessage &idl, core::logging::Message *native) noexcept
    {
        *native = decoded_logmessage(idl);
    }

    core::logging::Message decoded_logmessage(const CC::Status::LogMessage &idl) noexcept
    {
        auto level = decoded_level(idl.level());

        core::logging::Scope::ptr scope =
            !idl.log_scope().empty()
                ? core::logging::Scope::create(idl.log_scope(), level)
                : log_scope;

        return {
            idl.text(),
            level,
            scope,
            idl.origin(),
            decoded<core::dt::TimePoint>(idl.timestamp()),
            idl.filename(),
            idl.lineno(),
            idl.function(),
            static_cast<pid_t>(idl.thread_id()),
            idl.thread_name(),
            idl.task_name(),
            idl.host(),
            decoded<core::types::KeyValueMap>(idl.attributes()),
        };
    }

}  // namespace idl
