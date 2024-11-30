/// -*- c++ -*-
//==============================================================================
/// @file jsonfilesink.h++
/// @brief Log to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "asynclogsink.h++"
#include "rotatingpath.h++"
#include "factory.h++"
#include "parsers/json/writer.h++"
#include "types/filesystem.h++"
#include "types/create-shared.h++"

#include <filesystem>
#include <fstream>
#include <mutex>

namespace core::logging
{
    //--------------------------------------------------------------------------
    // JsonFileSink

    class JsonFileSink : public AsyncLogSink,
                         public RotatingPath,
                         public types::enable_create_shared<JsonFileSink>
    {
        using This = JsonFileSink;
        using Super = AsyncLogSink;

    protected:
        JsonFileSink(const std::string &sink_id);

        void load_settings(const types::KeyValueMap &settings) override;
        void open() override;
        void close() override;
        void open_file(const dt::TimePoint &tp) override;
        void close_file() override;
        void capture_event(const status::Event::ptr &event) override;

    private:
        std::shared_ptr<json::Writer> writer_;
    };

    //--------------------------------------------------------------------------
    // Add sink factory to enable `--log-to-json` option.

    inline static SinkFactory json_factory(
        "jsonfile",
        "Log each event as a single-line JSON object",
        [](const SinkID &sink_id) -> Sink::ptr {
            return JsonFileSink::create_shared(sink_id);
        });

}  // namespace core::logging
