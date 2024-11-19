/// -*- c++ -*-
//==============================================================================
/// @file csvfilesink.h++
/// @brief Log tabular data to file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "asynclogsink.h++"
#include "tabulardata.h++"
#include "rotatingpath.h++"
#include "factory.h++"
#include "messageformatter.h++"
#include "types/filesystem.h++"
#include "types/create-shared.h++"

#include <filesystem>
#include <fstream>
#include <mutex>

namespace core::logging
{
    const std::string SETTING_COL_SEP = "column separator";
    const std::string DEFAULT_COL_SEP = ",";

    //--------------------------------------------------------------------------
    // CSVFileSink

    class CSVFileSink : public AsyncLogSink,
                        public TabularData,
                        public RotatingPath,
                        public types::enable_create_shared<CSVFileSink>
    {
        using This = CSVFileSink;
        using Super = AsyncLogSink;

    protected:
        CSVFileSink(const std::string &sink_id);

        void load_settings(const types::KeyValueMap &settings) override;

        void open() override;
        void close() override;
        void open_file(const dt::TimePoint &tp) override;
        void close_file() override;
        void capture_event(const status::Event::ptr &event) override;

        void write_header();

        const std::string &separator() const;
        void set_separator(const std::string &separator);
        std::string protect_separator(std::string &&field) const;

    private:
        std::string separator_;
        std::ofstream stream;
    };

    //--------------------------------------------------------------------------
    // Add sink factory to enable `--log-to-csv` option.

    inline static SinkFactory csv_factory(
        "csv",
        "Log to a CSV file, capturing specific event fields per column",
        [](const SinkID &sink_id) -> Sink::ptr {
            return CSVFileSink::create_shared(sink_id);
        });

}  // namespace core::logging
