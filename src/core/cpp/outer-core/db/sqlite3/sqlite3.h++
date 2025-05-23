/// -*- c++ -*-
//==============================================================================
/// @file sqlite3.h++
/// @brief Wrapper for Sqlite3 C library
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/filesystem.h++"
#include "types/value.h++"
#include "thread/blockingqueue.h++"

#include "sqlite3.h"

namespace core::db
{
    class SQLite3
    {
    public:
        using ColumnNames = std::vector<std::string>;
        using RowData = core::types::ValueList;
        using MultiRowData = std::vector<RowData>;

    protected:
        using QueryCallbackFunction = std::function<bool(core::types::TaggedValueList &&)>;
        using QueryResponseQueue = core::types::BlockingQueue<core::types::TaggedValueList>;

    public:
        SQLite3();
        ~SQLite3();

        bool is_open() const;
        fs::path db_file() const;
        void open(const fs::path &db_file);
        void close(bool check_status = false);

        void execute(
            const std::string &sql,
            const QueryCallbackFunction &callback = {});

        void execute(
            const std::string &sql,
            const RowData &parameters,
            const QueryCallbackFunction &callback = {});

        // Invoke SQL statement with multiple input rows
        void execute_multi(
            const std::string &sql,
            const MultiRowData &parameters,
            const QueryCallbackFunction &callback = {});

        std::shared_ptr<QueryResponseQueue> execute_async_query(
            const std::string &sql,
            const RowData &parameters = {},
            std::size_t queue_size = 4096);

    protected:
        ::sqlite3 *connection() const;

        void bind_input_parameters(
            ::sqlite3_stmt *statement,
            const RowData &parameters) const;

        void execute_statement(
            ::sqlite3_stmt *statement,
            const QueryCallbackFunction &callback);

        ColumnNames column_names(
            ::sqlite3_stmt *statement) const;

        bool process_row(
            ::sqlite3_stmt *statement,
            const ColumnNames &column_names,
            const QueryCallbackFunction &callback) const;

        core::types::TaggedValueList extract_row(
            ::sqlite3_stmt *statement,
            const ColumnNames &column_names) const;

    protected:
        void check_status(
            int code,
            std::string &&action = {},
            core::types::KeyValueMap &&attributes = {}) const;

    private:
        ::sqlite3 *connection_;
        fs::path db_file_;
        std::mutex db_lock_;
    };
}  // namespace core::db
