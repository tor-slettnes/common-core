/// -*- c++ -*-
//==============================================================================
/// @file sqlitesink.h++
/// @brief Log tabular data to SQLite3 database
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logging/sinks/asynclogsink.h++"
#include "logging/sinks/tabulardata.h++"
#include "logging/sinks/rotatingpath.h++"
#include "sqlite3.h++"
#include "types/filesystem.h++"
#include "types/create-shared.h++"

#include <filesystem>
#include <fstream>
#include <mutex>

namespace logger
{
    const std::string SETTING_BATCH_SIZE = "batch size";
    const std::size_t DEFAULT_BATCH_SIZE = 64;

    const std::string SETTING_BATCH_TIMEOUT = "batch timeout";
    const std::size_t DEFAULT_BATCH_TIMEOUT = 5;

    const std::string SETTING_TABLE_NAME = "table name";
    const std::string DEFAULT_TABLE_NAME = "Events";

    //--------------------------------------------------------------------------
    // SQLiteSink

    class SQLiteSink : public core::logging::AsyncLogSink,
                       public core::logging::TabularData,
                       public core::logging::RotatingPath,
                       public core::db::SQLite3,
                       public core::types::enable_create_shared<SQLiteSink>
    {
        using This = SQLiteSink;
        using Super = core::logging::AsyncLogSink;

    protected:
        SQLiteSink(const std::string &sink_id);

    protected:
        void load_settings(const core::types::KeyValueMap &settings) override;
        std::optional<core::logging::ColumnSpec> column_spec(
            const core::types::Value &column_data) override;

        void open() override;
        void close() override;
        void open_file(const core::dt::TimePoint &tp) override;
        void close_file() override;
        void capture_event(const core::status::Event::ptr &event) override;

        void create_table();

    protected:
        std::string table_name;
        std::string placeholders;
        std::thread worker_thread;
        core::types::BlockingQueue<RowData> queue;
    };
}  // namespace logger
