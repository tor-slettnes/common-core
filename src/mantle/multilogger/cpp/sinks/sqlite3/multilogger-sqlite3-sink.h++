/// -*- c++ -*-
//==============================================================================
/// @file multilogger-sqlite3-sink.h++
/// @brief Log tabular data to SQLite3 database
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sqlite3.h++"
#include "logging/sinks/sink.h++"
#include "logging/sinks/tabulardata.h++"
#include "logging/sinks/rotatingpath.h++"
#include "logging/sinks/factory.h++"
#include "chrono/date-time.h++"
#include "thread/blockingqueue.h++"
#include "types/filesystem.h++"
#include "types/create-shared.h++"

#include <filesystem>
#include <fstream>
#include <mutex>

namespace multilogger
{
    const std::string SETTING_BATCH_SIZE = "batch size";
    const std::size_t DEFAULT_BATCH_SIZE = 64;

    const std::string SETTING_BATCH_TIMEOUT = "batch timeout";
    const std::size_t DEFAULT_BATCH_TIMEOUT = 5;

    const std::string SETTING_TABLE_NAME = "table name";
    const std::string DEFAULT_TABLE_NAME = "Events";

    //--------------------------------------------------------------------------
    // SQLiteSink

    class SQLiteSink : public core::logging::Sink,
                       public core::logging::TabularData,
                       public core::logging::RotatingPath,
                       public core::types::enable_create_shared<SQLiteSink>
    {
        using This = SQLiteSink;
        using Super = core::logging::Sink;

    protected:
        SQLiteSink(const std::string &sink_id);

    protected:
        void load_settings(const core::types::KeyValueMap &settings) override;
        void load_db_settings(const core::types::KeyValueMap &settings);

        std::string table_name() const;
        void set_table_name(const std::string &name);

        std::size_t batch_size() const;
        void set_batch_size(const std::size_t &size);

        core::dt::Duration batch_timeout() const;
        void set_batch_timeout(const core::dt::Duration &timeout);

        void open() override;
        void close() override;
        void open_file(const core::dt::TimePoint &tp) override;
        void close_file() override;

        void worker();
        bool handle_item(const core::types::Loggable::ptr &item) override;
        void flush();

        void create_table();
        void create_placeholders();

    private:
        std::string table_name_;
        std::size_t batch_size_;
        core::dt::Duration batch_timeout_;
        core::db::SQLite3 db;
        std::string placeholders;
        std::thread worker_thread_;
        core::db::SQLite3::MultiRowData pending_rows;
        core::types::BlockingQueue<core::types::Loggable::ptr> queue;
    };

    //--------------------------------------------------------------------------
    // Add sink factory to enable `--log-to-<>` option.

    inline static core::logging::SinkFactory sqlite3_factory(
        "sqlite3",
        "Log to a Sqlite3 database, capturing specific fields per column",
        [](const core::logging::SinkID &sink_id) -> core::logging::Sink::ptr
        {
            return SQLiteSink::create_shared(sink_id);
        });
} // namespace multilogger
