/// -*- c++ -*-
//==============================================================================
/// @file zmq-messagehandler.h++
/// @brief Process received message publications
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "zmq-endpoint.h++"  // for `log_scope`
#include "types/bytevector.h++"

namespace core::zmq
{
    using Filter = core::types::ByteVector;
    using MessageParts = std::vector<core::types::ByteVector>;

    class Subscriber;

    class MessageHandler : public std::enable_shared_from_this<MessageHandler>
    {
    public:
        using Identity = std::string;

    protected:
        MessageHandler(
            const std::string &id,
            const std::optional<Filter> &filter = {},
            const std::weak_ptr<Subscriber> &subscriber = {});

        ~MessageHandler();

    public:
        const Identity &id() const noexcept;
        const std::optional<Filter> &filter() const noexcept;

        virtual void initialize();
        virtual void deinitialize();

        // Subclasses should implement one of the following two methods to
        // process incoming message publications.

        // Override to capture each publication as a vector of individual parts.
        // (Least impactful).
        virtual void handle(const MessageParts &parts);

        // Override to capture each publication as a single byte vector combined
        // from the message parts.  The first part is skipped iff `filter` was
        // provided in the constructor, since these will then be identical.
        //
        // In other words, this will capture the message payload, without the
        // topic.
        virtual void handle(const core::types::ByteVector &data) {}

    protected:
        core::types::ByteVector combine_parts(
            const MessageParts &parts,
            bool remove_header = true) const;

    private:
        const std::string id_;
        const std::optional<Filter> filter_;
        std::weak_ptr<Subscriber> subscriber_;
    };

}  // namespace core::zmq
