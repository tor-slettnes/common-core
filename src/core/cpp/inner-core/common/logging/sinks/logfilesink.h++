/// -*- c++ -*-
//==============================================================================
/// @file logfilesink.h++
/// @brief Log to file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "asynclogsink.h++"
#include "messageformatter.h++"
#include "rotatingpath.h++"
#include "types/create-shared.h++"
#include "types/filesystem.h++"

#include <fstream>

namespace core::logging
{
    class LogFileSink : public AsyncLogSink,
                        public MessageFormatter,
                        public RotatingPath,
                        public types::enable_create_shared<LogFileSink>
    {
        using This = LogFileSink;
        using Super = AsyncLogSink;

    protected:
        LogFileSink(const std::string &sink_id);

        void load_settings(const types::KeyValueMap &settings) override;
        void open() override;
        void close() override;
        void open_file(const dt::TimePoint &tp) override;
        void close_file() override;
        void capture_event(const status::Event::ptr &event) override;

    private:
        std::shared_ptr<std::ofstream> stream_;
    };
}  // namespace core::logging
