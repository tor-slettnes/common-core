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
                     const std::string &origin,
                     Code code,
                     const Symbol &symbol,
                     const types::KeyValueMap &attributes,
                     const ContractID &contract_id,
                     const std::string &host)
        : Event(text,                         // text
                status::Domain::APPLICATION,  // domain
                origin,                       // origin
                code,                         // code
                symbol,                       // symbol
                level,                        // level
                tp,                           // timepoint
                attributes,                   // attributes
                contract_id,                  // contract_id
                host),                        // host
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

    void Message::populate_fields(types::PartsList *parts) const noexcept
    {
        Event::populate_fields(parts);
        parts->add_string(MESSAGE_FIELD_LOG_SCOPE, this->scopename(), "%s");
        parts->add_string(MESSAGE_FIELD_SOURCE_PATH, this->path().string());
        parts->add_value(MESSAGE_FIELD_SOURCE_LINE, this->lineno(), this->lineno() != 0);
        parts->add_string(MESSAGE_FIELD_FUNCTION_NAME, this->function());
        parts->add_value(MESSAGE_FIELD_THREAD_ID, this->thread_id());
    }

    status::Event::ptr Message::as_event() const noexcept
    {
        Event::ptr event = std::make_shared<Event>(*this);
        types::KeyValueMap &attributes = event->attributes();

        attributes.insert_if(
            this->scope() != nullptr,
            core::logging::MESSAGE_FIELD_LOG_SCOPE,
            this->scopename());

        attributes.insert_if(
            !this->path().empty(),
            core::logging::MESSAGE_FIELD_SOURCE_PATH,
            this->path());

        attributes.insert_if(
            this->lineno(),
            core::logging::MESSAGE_FIELD_SOURCE_LINE,
            this->lineno());

        attributes.insert_if(
            !this->function().empty(),
            core::logging::MESSAGE_FIELD_FUNCTION_NAME,
            this->function());

        attributes.insert_if(
            this->thread_id(),
            core::logging::MESSAGE_FIELD_THREAD_ID,
            this->thread_id());

        return event;
    }

    Message::ptr Message::create_from_event(const Event &event) noexcept
    {
        types::KeyValueMap attributes = event.attributes();
        types::Value scope_name = attributes.extract_value(MESSAGE_FIELD_LOG_SCOPE);
        types::Value source_path = attributes.extract_value(MESSAGE_FIELD_SOURCE_PATH);
        types::Value source_line = attributes.extract_value(MESSAGE_FIELD_SOURCE_LINE);
        types::Value fn_name = attributes.extract_value(MESSAGE_FIELD_FUNCTION_NAME);
        types::Value thread_id = attributes.extract_value(MESSAGE_FIELD_THREAD_ID);

        if (!event.text().empty() &&
            (scope_name || source_path || source_line || fn_name || thread_id))
        {
            Scope::ptr scope = logging::scopes.get(scope_name.as_string());
            if (!scope)
            {
                scope = Scope::create(scope_name.as_string());
            }

            return std::make_shared<Message>(
                event.text(),             // text
                event.level(),            // level
                scope,                    // scope
                event.timepoint(),        // tp
                source_path.as_string(),  // path
                source_line.as_uint(),    // lineno
                fn_name.as_string(),      // function
                thread_id.as_uint(),      // thread_id
                event.origin(),           // origin
                event.code(),             // code
                event.symbol(),           // symbol
                attributes,               // attributes
                event.contract_id(),      // contract_id
                event.host());            // host
        }
        else
        {
            return {};
        }
    }

    std::vector<std::string> Message::field_names() noexcept
    {
        return {
            core::status::EVENT_FIELD_TIME,
            core::status::EVENT_FIELD_HOST,
            core::status::EVENT_FIELD_LEVEL,
            MESSAGE_FIELD_LOG_SCOPE,
            MESSAGE_FIELD_THREAD_ID,
            MESSAGE_FIELD_SOURCE_PATH,
            MESSAGE_FIELD_SOURCE_LINE,
            MESSAGE_FIELD_FUNCTION_NAME,
            core::status::EVENT_FIELD_TEXT,
        };
    }

}  // namespace core::logging
