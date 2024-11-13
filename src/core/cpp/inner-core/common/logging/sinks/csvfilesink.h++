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

    class CSVBaseSink : public AsyncLogSink,
                        public TabularData,
                        public RotatingPath
    {
        using This = CSVBaseSink;
        using Super = AsyncLogSink;

    protected:
        CSVBaseSink(const std::string &sink_id);

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
        std::shared_ptr<std::ofstream> stream_;
    };

    //--------------------------------------------------------------------------
    // CSVEventSink

    class CSVEventSink : public CSVBaseSink,
                         public types::enable_create_shared<CSVEventSink>
    {
        using This = CSVEventSink;
        using Super = CSVBaseSink;

    protected:
        CSVEventSink(const std::string &sink_id);
    };

    //--------------------------------------------------------------------------
    // CSVMessageSink

    class CSVMessageSink : public CSVBaseSink,
                           public MessageFormatter,
                           public types::enable_create_shared<CSVMessageSink>
    {
        using This = CSVMessageSink;
        using Super = CSVBaseSink;

    protected:
        CSVMessageSink(const std::string &sink_id);

    protected:
        bool is_applicable(const types::Loggable &item) const override;
    };

}  // namespace core::logging
