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
                     const dt::TimePoint &tp,
                     const fs::path &path,
                     uint lineno,
                     const std::string &function,
                     pid_t thread_id,
                     const std::string &thread_name,
                     const std::string &task_name,
                     const std::string &host,
                     const std::string &origin,
                     const types::KeyValueMap &attributes)
        : Super(text, level, origin, tp, attributes),
          scope_(scope),
          path_(path),
          lineno_(lineno),
          function_(function),
          thread_id_(thread_id),
          thread_name_(thread_name),
          task_name_(task_name),
          host_(host)
    {
    }

    Message::Message(const Message &other)
        : Super(other),
          scope_(other.scope()),
          path_(other.path()),
          lineno_(other.lineno()),
          function_(other.function()),
          thread_id_(other.thread_id()),
          thread_name_(other.thread_name()),
          task_name_(other.task_name()),
          host_(other.host())
    {
    }

    Message &Message::operator=(Message &&other) noexcept
    {
        Super::operator=(std::move(other));
        std::swap(this->scope_, other.scope_);
        std::swap(this->path_, other.path_);
        std::swap(this->lineno_, other.lineno_);
        std::swap(this->function_, other.function_);
        std::swap(this->thread_id_, other.thread_id_);
        std::swap(this->thread_name_, other.thread_name_);
        std::swap(this->task_name_, other.task_name_);
        std::swap(this->host_, other.host_);
        return *this;
    }

    Message &Message::operator=(const Message &other) noexcept
    {
        Super::operator=(other);
        this->scope_ = other.scope();
        this->path_ = other.path();
        this->lineno_ = other.lineno();
        this->function_ = other.function();
        this->thread_id_ = other.thread_id();
        this->host_ = other.host();
        return *this;
    }

    bool Message::operator==(const Message &other) const noexcept
    {
        return Super::operator==(other) &&
               (this->scopename() == other.scopename()) &&
               (this->path() == other.path()) &&
               (this->lineno() == other.lineno()) &&
               (this->function() == other.function()) &&
               (this->thread_id() == other.thread_id()) &&
               (this->host() == other.host());
    }

    std::string Message::class_name() const noexcept
    {
        return "Message";
    }

    // void Message::to_stream(std::ostream &stream) const
    // {
    //     stream << this->text();
    // }

    bool Message::is_applicable() const noexcept
    {
        if (this->level() == status::Level::NONE)
        {
            return false;
        }
        else if (const Scope::ptr &scope = this->scope())
        {
            return scope->is_applicable(this->level());
        }
        else
        {
            return false;
        }
    }

    std::string Message::contract_id() const noexcept
    {
        return This::CONTRACT;
    }

    Scope::ptr Message::scope() const noexcept
    {
        return this->scope_;
    }

    std::string Message::scopename_or(const std::string &fallback) const noexcept
    {
        return this->scope() ? this->scope()->name : fallback;
    }

    std::string Message::scopename() const noexcept
    {
        return this->scopename_or("");
    }

    const fs::path &Message::path() const noexcept
    {
        return this->path_;
    }

    uint Message::lineno() const noexcept
    {
        return this->lineno_;
    }

    const std::string &Message::function() const noexcept
    {
        return this->function_;
    }

    pid_t Message::thread_id() const noexcept
    {
        return this->thread_id_;
    }

    std::string Message::thread_name() const noexcept
    {
        return this->thread_name_;
    }

    std::string Message::task_name() const noexcept
    {
        return this->task_name_;
    }

    std::string Message::host() const noexcept
    {
        return this->host_;
    }

    std::vector<std::string> Message::field_names() const noexcept
    {
        return This::message_fields();
    }

    std::vector<std::string> Message::message_fields() noexcept
    {
        std::vector<std::string> fields = Super::event_fields();
        fields.insert(
            fields.end(),
            {
                This::FIELD_LOG_SCOPE,
                This::FIELD_SOURCE_PATH,
                This::FIELD_SOURCE_LINE,
                This::FIELD_FUNCTION_NAME,
                This::FIELD_THREAD_ID,
                This::FIELD_THREAD_NAME,
                This::FIELD_TASK_NAME,
                This::FIELD_HOST,
            });

        return fields;
    }

    types::Value Message::get_field_as_value(
        const std::string &field_name) const
    {
        using LookupFunction = std::function<types::Value(const Message *message)>;
        static const std::unordered_map<std::string, LookupFunction> lookup = {
            {This::FIELD_LOG_SCOPE, [=](const Message *message) {
                 return message->scopename();
             }},
            {This::FIELD_SOURCE_PATH, [=](const Message *message) {
                 return message->path();
             }},
            {This::FIELD_SOURCE_LINE, [=](const Message *message) {
                 return message->lineno();
             }},
            {This::FIELD_FUNCTION_NAME, [=](const Message *message) {
                 return message->function();
             }},
            {This::FIELD_HOST, [=](const Message *message) {
                 return message->host();
             }},
            {This::FIELD_THREAD_ID, [=](const Message *message) {
                 return message->thread_id();
             }},
            {This::FIELD_THREAD_NAME, [=](const Message *message) {
                 return message->thread_name();
             }},
            {This::FIELD_TASK_NAME, [=](const Message *message) {
                 return message->task_name();
             }},
        };

        try
        {
            return lookup.at(field_name)(this);
        }
        catch (const std::out_of_range &)
        {
            return Super::get_field_as_value(field_name);
        }
    }

}  // namespace core::logging
