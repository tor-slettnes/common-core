/// -*- c++ -*-
//==============================================================================
/// @file csvfilesink.h++
/// @brief Log tabular data to file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "tabulardatasink.h++"
#include "rotatingpath.h++"
#include "json/writer.h++"
#include "types/filesystem.h++"
#include "types/create-shared.h++"

#include <filesystem>
#include <fstream>
#include <mutex>

namespace core::logging
{
    //--------------------------------------------------------------------------
    // CSVFileSink

    class CSVBaseSink : public TabularDataSink,
                        public RotatingPath
    {
        using This = CSVBaseSink;
        using Super = TabularDataSink;

    protected:
        CSVBaseSink(const std::string &sink_id,
                    status::Level threshold,
                    const std::optional<std::string> &contract_id,
                    const ColumnDefaults &columns,
                    const std::string &path_template,
                    const dt::DateTimeInterval &rotation_interval,
                    bool local_time = true,
                    const std::string &separator = ",");

    protected:
        const std::string &separator() const;
        void open() override;
        void close() override;
        void open_file(const dt::TimePoint &tp) override;
        void close_file() override;
        void capture_event(const status::Event::ptr &event) override;

        void write_header();
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
    protected:
        using CSVBaseSink::CSVBaseSink;
    };

    //--------------------------------------------------------------------------
    // CSVMessageSink

    class CSVMessageSink : public CSVBaseSink,
                           public types::enable_create_shared<CSVMessageSink>
    {
        using This = CSVMessageSink;
        using Super = CSVBaseSink;

    protected:
        CSVMessageSink(const std::string &sink_id,
                       status::Level threshold,
                       const std::vector<std::string> &column_names,
                       const std::string &path_template,
                       const dt::DateTimeInterval &rotation_interval,
                       bool local_time = true,
                       const std::string &separator = ",");

    protected:
        bool is_applicable(const types::Loggable &item) const override;

    private:
        static types::TaggedValueList message_columns(
            const std::vector<std::string> &column_names);
    };

}  // namespace core::logging
