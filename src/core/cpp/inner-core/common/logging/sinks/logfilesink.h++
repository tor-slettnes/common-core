/// -*- c++ -*-
//==============================================================================
/// @file logfilesink.h++
/// @brief Log to file
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "messagesink.h++"
#include "async-wrapper.h++"
#include "rotatingpath.h++"
#include "factory.h++"
#include "types/create-shared.h++"
#include "types/filesystem.h++"

#include <fstream>

namespace core::logging
{
    class LogFileSink : public AsyncWrapper<MessageSink>,
                        public RotatingPath,
                        public types::enable_create_shared<LogFileSink>
    {
        using This = LogFileSink;
        using Super = AsyncWrapper<MessageSink>;

    protected:
        LogFileSink(const std::string &sink_id);

    protected:
        void load_settings(const types::KeyValueMap &settings) override;
        void open() override;
        void close() override;
        void open_file(const dt::TimePoint &tp) override;
        void close_file() override;
        bool handle_message(const Message::ptr &message) override;

    private:
        std::shared_ptr<std::ofstream> stream_;
    };

    //--------------------------------------------------------------------------
    // Add sink factory to enable `--log-to-file` option.

    inline static SinkFactory file_factory(
        "logfile",
        "Log to a plain log file",
        [](const SinkID &sink_id) -> Sink::ptr
        {
            return LogFileSink::create_shared(sink_id);
        });

}  // namespace core::logging
