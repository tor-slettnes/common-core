/// -*- c++ -*-
//==============================================================================
/// @file streamsink.h++
/// @brief Log sink for stream-based backend, (stdout/stderr, file, ...)
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logsink.h++"
#include "messageformatter.h++"
#include "types/create-shared.h++"
#include "status/level.h++"
#include "types/value.h++"
#include "types/valuemap.h++"

#include <ostream>
#include <string>

namespace core::logging
{
    //==========================================================================
    /// \class StreamSink
    /// \brief Abstract base for logging to output streams (stdout, stderr, file)

    class StreamSink : public LogSink,
                       public MessageFormatter,
                       public types::enable_create_shared<StreamSink>
    {
        enum MessagePart
        {
            INTRO,
            TEXT,
            RESET
        };

        using StyleMap = types::ValueMap<MessagePart, std::string>;
        using Super = LogSink;

    protected:
        /// \brief Constructor
        /// \param[in] stream
        ///     Stream to log the messages to
        StreamSink(const std::string &sink_id,
                   std::ostream &stream);

        StreamSink(const std::string &sink_id,
                   std::ostream &&stream);

    public:
        void load_settings(const types::KeyValueMap &settings) override;
        bool is_applicable(const types::Loggable &item) const override;
        void capture_event(const status::Event::ptr &event) override;

    private:
        void load_styles(const types::KeyValueMap &stylemap);
        void load_styles(const types::KeyValueMap &stylemap,
                         MessagePart part,
                         const std::string &partname);

    protected:
        std::ostream &stream;
        types::ValueMap<status::Level, StyleMap> styles;
    };
}  // namespace core::logging
