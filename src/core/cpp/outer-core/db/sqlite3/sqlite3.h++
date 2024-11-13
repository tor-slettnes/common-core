/// -*- c++ -*-
//==============================================================================
/// @file sqlite3.h++
/// @brief Wrapper for Sqlite3 C library
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sqlbase.h++"
#include "sqlite3.h"

namespace core::db
{
    class SQLite3: public SQLBase
    {
    public:
        SQLite3();
        ~SQLite3();

        bool is_open() const;
        fs::path db_file() const;
        void open(const fs::path &db_file);
        void close();

        // Invoke SQL statement with multiple input rows
        void execute_multi(
            const std::string &sql,
            const MultiRowData &parameters,
            const QueryCallbackFunction &callback = {}) const override;

    protected:
        ::sqlite3 *connection() const;

        void bind_input_parameters(
            ::sqlite3_stmt *statement,
            const RowData &parameters) const;

        void execute_statement(
            ::sqlite3_stmt *statement,
            const QueryCallbackFunction &callback) const;

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
            const core::types::KeyValueMap &attributes = {}) const;

    private:
        ::sqlite3 *connection_;
        fs::path db_file_;
    };
}  // namespace core::sqlite3
