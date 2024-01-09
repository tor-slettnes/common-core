/// -*- c++ -*-
//==============================================================================
/// @file message.c++
/// @brief A log message
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "message.h++"
#include "platform/process.h++"

namespace cc::logging
{
    //==========================================================================
    // Message

    Message::Message(Scope::Ref scope,
                     status::Level level,
                     const dt::TimePoint &tp,
                     const fs::path &path,
                     uint lineno,
                     const std::string &function,
                     pid_t thread_id,
                     const std::string &origin,
                     const types::KeyValueMap &attributes,
                     const std::string &text)
        : Event(text, status::Domain::APPLICATION, origin, level, tp, attributes),
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

    void Message::populate_fields(types::TaggedValueList *values) const noexcept
    {
        Event::populate_fields(values);
        values->reserve(values->size() + 5);

        if (this->scope())
        {
            values->emplace_back(MESSAGE_FIELD_LOG_SCOPE, this->scopename());
        }

        if (const fs::path &path = this->path(); !this->path().empty())
        {
            values->emplace_back(MESSAGE_FIELD_SOURCE_PATH, path.string());
        }

        if (uint lineno = this->lineno())
        {
            values->emplace_back(MESSAGE_FIELD_SOURCE_LINE, this->lineno());
        }

        if (const std::string &fn = this->function(); !fn.empty())
        {
            values->emplace_back(MESSAGE_FIELD_FUNCTION_NAME, fn);
        }

        if (pid_t tid = this->thread_id())
        {
            values->emplace_back(MESSAGE_FIELD_THREAD_ID, tid);
        }
    }

}  // namespace cc::logging
