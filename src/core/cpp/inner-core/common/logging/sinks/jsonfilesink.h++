/// -*- c++ -*-
//==============================================================================
/// @file jsonfilesink.h++
/// @brief Log to JSON file
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "sink.h++"
#include "async-wrapper.h++"
#include "rotatingpath.h++"
#include "factory.h++"
#include "parsers/json/writer.h++"
#include "types/filesystem.h++"
#include "types/create-shared.h++"

#include <filesystem>
#include <fstream>
#include <mutex>

namespace cc::core::logging
{
    constexpr auto SETTING_KEYS = "keys";

    //--------------------------------------------------------------------------
    // JsonFileSink

    class JsonFileSink : public AsyncWrapper<Sink>,
                         public RotatingPath,
                         public types::enable_create_shared<JsonFileSink>
    {
        using This = JsonFileSink;
        using Super = AsyncWrapper<Sink>;

    protected:
        JsonFileSink(const std::string &sink_id);

    protected:
        void load_settings(const types::KeyValueMap &settings) override;
        void load_keys(const types::KeyValueMap &settings);

        const std::vector<std::string> &keys() const;
        void set_keys(std::vector<std::string> keys);

        void open() override;
        void close() override;
        void open_file(const dt::TimePoint &tp) override;
        void close_file() override;
        bool handle_item(const types::Loggable::ptr &loggable) override;

    private:
        std::shared_ptr<json::Writer> writer_;
        std::vector<std::string> keys_;
    };

    //--------------------------------------------------------------------------
    // Add sink factory to enable `--log-to-json` option.

    inline static SinkFactory json_factory(
        "jsonfile",
        "Log each message as a single-line JSON object",
        [](const SinkID &sink_id) -> Sink::ptr {
            return JsonFileSink::create_shared(sink_id);
        });

}  // namespace cc::core::logging
