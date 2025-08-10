/// -*- c++ -*-
//==============================================================================
/// @file sqlite3.h++
/// @brief Wrapper for Sqlite3 C library
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sql-common.h++"
#include "types/filesystem.h++"
#include "types/value.h++"
#include "thread/blockingqueue.h++"

#include "sqlite3.h"

namespace core::db
{
    class SQLite3 : public SQL
    {
        using This = SQLite3;

    public:
        using ColumnName = std::string;
        using ColumnNames = std::vector<ColumnName>;
        using RowData = core::types::ValueList;
        using MultiRowData = std::vector<RowData>;

        struct ColumnSpec
        {
            std::string name;
            core::types::ValueType type = core::types::ValueType::NONE;
        };

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

        std::vector<ColumnSpec> columns(
            const std::string &table_name) const;

        std::vector<std::string> column_names(
            const std::string &table_name) const override;

        std::size_t column_count(
            const std::string &table_name) const override;

        void create_table(
            const std::string &table_name,
            const std::vector<ColumnSpec> &columns);

        void read(
            const QueryCallbackFunction &callback,
            const std::string &table_name,
            const std::vector<std::string> &columns = ALL_COLUMNS,
            const std::vector<std::string> &conditions = {},
            const std::string &order_by = {},
            SortDirection direction = SortDirection::ASCENDING,
            uint limit = 0);

        void insert_multi(
            const std::string &table_name,
            const MultiRowData &parameters,
            const QueryCallbackFunction &callback = {});

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

        std::string get_placeholders(
            const std::string &table_name) const;

    protected:
        ::sqlite3 *connection() const;
        ::sqlite3_stmt *statement(const std::string &sql) const;
        ::sqlite3_stmt *select_all_from(const std::string &table_name) const;
        void finalize(::sqlite3_stmt *statement) const;

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

    public:
        // static core::types::ValueMap<std::string, core::types::ValueType> SQLite3::column_type_mapping;
        static types::SymbolMap<core::types::ValueType> column_type_names;

    private:
        ::sqlite3 *connection_;
        fs::path db_file_;
        std::mutex db_lock_;
    };
}  // namespace core::db
