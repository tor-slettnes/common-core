/// -*- c++ -*-
//==============================================================================
/// @file messagesink.h++
/// @brief Log sink for text messages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sink.h++"
#include "logging/message/scope.h++"
#include "logging/message/message.h++"

#include <string>

namespace core::logging
{
    //==========================================================================
    /// @class MessageSink
    /// @brief Base for logging text

    class MessageSink : public Sink
    {
        using This = MessageSink;
        using Super = Sink;

    public:
        using ptr = std::shared_ptr<MessageSink>;

    protected:
        MessageSink(const SinkID &sink_id, bool asynchronous = false);

    public:
        void load_settings(const types::KeyValueMap &settings) override;

    protected:
        virtual void set_include_context(bool include_context);
        bool include_context() const;

    public:
        static void set_all_include_context(bool include_context);

    protected:
        bool is_applicable(const types::Loggable &item) const override;

        bool handle_item(const types::Loggable::ptr &item) override;
        virtual bool handle_message(const Message::ptr &message) = 0;

        void send_preamble(std::ostream &stream,
                           const Message::ptr &message) const;

        std::string formatted(const Message::ptr &message) const;
    private:
        status::Level threshold_;
        bool include_context_;
        static std::optional<bool> all_include_context_;
    };
}  // namespace core::logging
