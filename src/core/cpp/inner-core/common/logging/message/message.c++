/// -*- c++ -*-
//==============================================================================
/// @file message.c++
/// @brief A log message
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "message.h++"
#include "platform/process.h++"

namespace core::logging
{
    //==========================================================================
    // Message

    Message::Message(const std::string &text,
                     status::Level level,
                     Scope::ptr scope,
                     status::Flow flow,
                     const dt::TimePoint &tp,
                     const fs::path &path,
                     uint lineno,
                     const std::string &function,
                     pid_t thread_id,
                     const std::string &origin,
                     Code code,
                     const Symbol &symbol,
                     const types::KeyValueMap &attributes)
        : Event(text,
                status::Domain::APPLICATION,
                origin,
                code,
                symbol,
                level,
                flow,
                tp,
                attributes),
          scope_(scope),
          path_(path),
          lineno_(lineno),
          function_(function),
          thread_id_(thread_id)
    {
    }

    Message::Message(const Message &other)
        : Event(other),
          scope_(other.scope()),
          path_(other.path()),
          lineno_(other.lineno()),
          function_(other.function()),
          thread_id_(other.thread_id())
    {
    }

    Message &Message::operator=(const Message &other) noexcept
    {
        Event::operator=(other);
        this->scope_ = other.scope();
        this->path_ = other.path();
        this->lineno_ = other.lineno();
        this->function_ = other.function();
        this->thread_id_ = other.thread_id();
        return *this;
    }

    void Message::populate_fields(types::PartsList *parts) const noexcept
    {
        Event::populate_fields(parts);
        parts->add_string(MESSAGE_FIELD_LOG_SCOPE, this->scopename(), "%s");
        parts->add_string(MESSAGE_FIELD_SOURCE_PATH, this->path().string());
        parts->add_value(MESSAGE_FIELD_SOURCE_LINE, this->lineno(), this->lineno() != 0);
        parts->add_string(MESSAGE_FIELD_FUNCTION_NAME, this->function());
        parts->add_value(MESSAGE_FIELD_THREAD_ID, this->thread_id());
    }

}  // namespace core::logging
