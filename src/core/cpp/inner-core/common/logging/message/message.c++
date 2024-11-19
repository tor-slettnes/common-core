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
                     status::Domain domain,
                     const std::string &origin,
                     Code code,
                     const Symbol &symbol,
                     const types::KeyValueMap &attributes,
                     const ContractID &contract_id,
                     const std::string &host)
        : Event(text, domain, origin, code, symbol, level, tp, attributes, contract_id, host),
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

    Message &Message::operator=(Message &&other) noexcept
    {
        Event::operator=(std::move(other));
        std::swap(this->scope_, other.scope_);
        std::swap(this->path_, other.path_);
        std::swap(this->lineno_, other.lineno_);
        std::swap(this->function_, other.function_);
        std::swap(this->thread_id_, other.thread_id_);
        return *this;
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
        if (const Scope::ptr &scope = this->scope())
        {
            return scope->is_applicable(this->level());
        }
        else
        {
            return false;
        }
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

    std::vector<std::string> Message::field_names() const noexcept
    {
        return This::message_fields();
    }

    std::vector<std::string> Message::message_fields() noexcept
    {
        std::vector<std::string> fields = Event::event_fields();
        fields.insert(
            fields.end(),
            {
                MESSAGE_FIELD_THREAD_ID,
                MESSAGE_FIELD_LOG_SCOPE,
                MESSAGE_FIELD_SOURCE_PATH,
                MESSAGE_FIELD_SOURCE_LINE,
                MESSAGE_FIELD_FUNCTION_NAME,
            });

        return fields;
    }

    types::Value Message::get_field_as_value(
        const std::string &field_name) const
    {
        using LookupFunction = std::function<types::Value(const Message *message)>;
        static const std::unordered_map<std::string, LookupFunction> lookup = {
            {MESSAGE_FIELD_THREAD_ID, [=](const Message *message) {
                 return message->thread_id();
             }},
            {MESSAGE_FIELD_LOG_SCOPE, [=](const Message *message) {
                 return message->scopename();
             }},
            {MESSAGE_FIELD_SOURCE_PATH, [=](const Message *message) {
                 return message->path();
             }},
            {MESSAGE_FIELD_SOURCE_LINE, [=](const Message *message) {
                 return message->lineno();
             }},
            {MESSAGE_FIELD_FUNCTION_NAME, [=](const Message *message) {
                 return message->function();
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
