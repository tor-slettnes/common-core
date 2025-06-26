/// -*- c++ -*-
//==============================================================================
/// @file sqlite3.c++
/// @brief Wrapper for Sqlite3 C library
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sqlite3.h++"
#include "status/exceptions.h++"
#include "parsers/json/writer.h++"
#include "chrono/date-time.h++"
#include "types/value.h++"
#include "logging/logging.h++"

#include <functional>

namespace core::db
{
    SQLite3::SQLite3()
        : connection_(nullptr)
    {
    }

    SQLite3::~SQLite3()
    {
        this->close(false);
    }

    bool SQLite3::is_open() const
    {
        return !this->db_file_.empty();
    }

    fs::path SQLite3::db_file() const
    {
        return this->db_file_;
    }

    void SQLite3::open(const fs::path &db_file)
    {
        if (this->db_file_ != db_file)
        {
            this->close();
            this->db_file_ = db_file;

            this->check_status(sqlite3_open_v2(
                db_file.string().c_str(),
                &this->connection_,
                SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_EXRESCODE,
                nullptr));
        }
    }

    void SQLite3::close(bool check_status)
    {
        if (this->is_open())
        {
            std::scoped_lock lck(this->db_lock_);
            if (int status = sqlite3_close(this->connection_))
            {
                if (check_status)
                {
                    this->check_status(status);
                }
            }
            this->connection_ = nullptr;
            this->db_file_.clear();
        }
    }

    void SQLite3::execute(
        const std::string &sql,
        const QueryCallbackFunction &callback)
    {
        this->execute(sql, {}, callback);
    }

    void SQLite3::execute(
        const std::string &sql,
        const RowData &parameters,
        const QueryCallbackFunction &callback)
    {
        this->execute_multi(sql, {parameters}, callback);
    }

    void SQLite3::execute_multi(
        const std::string &sql,
        const MultiRowData &parameter_rows,
        const QueryCallbackFunction &callback)
    {
        std::scoped_lock lck(this->db_lock_);
        sqlite3_stmt *statement = nullptr;
        this->check_status(
            sqlite3_prepare_v2(
                this->connection(), // db
                sql.c_str(),        // zSql
                sql.size() + 1,     // nBytes
                &statement,         // ppStmt
                nullptr),           // pzTail
            "sqlite3_prepare",
            {{"sql", sql}});

        try
        {
            for (const RowData &parameters : parameter_rows)
            {
                this->bind_input_parameters(statement, parameters);
                this->execute_statement(statement, callback);

                this->check_status(sqlite3_reset(statement),
                                   "sqlite3_reset");

                this->check_status(sqlite3_clear_bindings(statement),
                                   "sqlite3_clear_bindings");
            }
        }
        catch (...)
        {
            sqlite3_finalize(statement);
            throw;
        }

        this->check_status(sqlite3_finalize(statement), "sqlite3_finalize");
    }
 
   std::shared_ptr<SQLite3::QueryResponseQueue> SQLite3::execute_async_query(
        const std::string &sql,
        const RowData &parameters,
        std::size_t queue_size)
    {
        using namespace std::placeholders;

        auto queue = std::make_shared<QueryResponseQueue>(
            queue_size,
            QueryResponseQueue::OverflowDisposition::BLOCK);

        std::thread executor_thread(
            [=]
            {
                this->execute(
                    sql,
                    parameters,
                    [=](core::types::TaggedValueList &&row) -> bool
                    {
                        return queue->put(row);
                    });
                queue->close();
            });

        executor_thread.detach();
        return queue;
    }

    ::sqlite3 *SQLite3::connection() const
    {
        if (!this->is_open())
        {
            throw core::exception::FailedPrecondition("No connection to SQLite3 database");
        }
        return this->connection_;
    }

    void SQLite3::bind_input_parameters(::sqlite3_stmt *statement,
                                        const RowData &parameters) const
    {
        for (int index = 0; index < parameters.size(); index++)
        {
            const core::types::Value &value = parameters.at(index);
            int status = SQLITE_OK;

            switch (value.type())
            {
            case core::types::ValueType::NONE:
                status = sqlite3_bind_null(statement,
                                           index + 1);
                break;

            case core::types::ValueType::BOOL:
            case core::types::ValueType::CHAR:
            case core::types::ValueType::UINT:
            case core::types::ValueType::SINT:
            {
                auto numeric_value = value.as_largest_sint();
                if ((numeric_value < std::numeric_limits<std::int64_t>::min()) ||
                    (numeric_value > std::numeric_limits<std::int64_t>::max()))
                {
                    status = sqlite3_bind_double(statement,
                                                 index + 1,
                                                 value.as_double());
                }
                else if ((numeric_value < std::numeric_limits<int>::min()) ||
                         (numeric_value > std::numeric_limits<int>::max()))
                {
                    status = sqlite3_bind_int64(statement,
                                                index + 1,
                                                numeric_value);
                }
                else
                {
                    status = sqlite3_bind_int(statement,
                                              index + 1,
                                              value.as_sint32());
                }
                break;
            }

            case core::types::ValueType::REAL:
            case core::types::ValueType::DURATION:
            case core::types::ValueType::TIMEPOINT:
                status = sqlite3_bind_double(statement,
                                             index + 1,
                                             value.as_double());
                break;

            case core::types::ValueType::STRING:
                if (auto *string = value.get_if<std::string>())
                {
                    sqlite3_bind_text(statement,
                                      index + 1,
                                      string->data(),
                                      string->size(),
                                      SQLITE_STATIC);
                }
                break;

            case core::types::ValueType::BYTEVECTOR:
                if (auto *bytes = value.get_if<core::types::ByteVector>())
                {
                    status = sqlite3_bind_blob(statement,
                                               index + 1,
                                               bytes->data(),
                                               bytes->size(),
                                               SQLITE_STATIC);
                }
                break;

            default:
                if (std::string encoded = core::json::writer.encoded(value); !encoded.empty())
                {
                    status = sqlite3_bind_text(statement,
                                               index + 1,
                                               encoded.data(),
                                               encoded.size(),
                                               SQLITE_TRANSIENT);
                }
                else
                {
                    status = sqlite3_bind_null(statement, index + 1);
                }
                break;
            }

            this->check_status(status,
                               "sqlite3_bind",
                               {
                                   {"parameter index", index},
                                   {"parameter value", value},
                               });
        }
    }

    void SQLite3::execute_statement(::sqlite3_stmt *statement,
                                    const QueryCallbackFunction &callback)
    {
        bool done = false;
        ColumnNames column_names = this->column_names(statement);

        while (!done)
        {
            switch (int statuscode = ::sqlite3_step(statement))
            {
                // case SQLITE_BUSY:
                //     break;

            case SQLITE_ROW:
                if (callback)
                {
                    this->process_row(statement, column_names, callback);
                }
                break;

            case SQLITE_OK:
            case SQLITE_DONE:
                done = true;
                break;

            default:
                this->check_status(statuscode, "sqlite3_step");
                break;
            }
        }
    }

    SQLite3::ColumnNames SQLite3::column_names(::sqlite3_stmt *statement) const
    {
        int column_count = sqlite3_column_count(statement);
        ColumnNames column_names(column_count);
        for (int col_index = 0; col_index < column_count; col_index++)
        {
            if (const char *name = sqlite3_column_name(statement, col_index))
            {
                column_names.at(col_index).assign(name);
            }
        }
        return column_names;
    }

    bool SQLite3::process_row(
        ::sqlite3_stmt *statement,
        const ColumnNames &column_names,
        const QueryCallbackFunction &callback) const
    {
        try
        {
            return callback(this->extract_row(
                statement,
                column_names));
        }
        catch (...)
        {
            logf_error("SQLite3 query callback from DB row #%d failed: %s",
                       this->db_file(),
                       std::current_exception());
            return false;
        }
    }

    core::types::TaggedValueList SQLite3::extract_row(
        ::sqlite3_stmt *statement,
        const ColumnNames &column_names) const
    {
        int ncols = sqlite3_column_count(statement);
        core::types::TaggedValueList row_data;
        row_data.reserve(ncols);
        for (int col_index = 0; col_index < ncols; col_index++)
        {
            const std::string &column_name = column_names.at(col_index);
            auto &[tag, value] = row_data.emplace_back(column_name, core::types::Value());

            switch (sqlite3_column_type(statement, col_index))
            {
            case SQLITE_INTEGER:
                value = sqlite3_column_int64(statement, col_index);
                break;

            case SQLITE_FLOAT:
                value = sqlite3_column_double(statement, col_index);
                break;

            case SQLITE_TEXT:
                if (const unsigned char *text = sqlite3_column_text(statement, col_index))
                {
                    int nbytes = sqlite3_column_bytes(statement, col_index);
                    value = std::string(reinterpret_cast<const char *>(text), nbytes);
                }
                break;

            case SQLITE_BLOB:
                if (const void *blob = sqlite3_column_blob(statement, col_index))
                {
                    auto *data = static_cast<const std::uint8_t *>(blob);
                    int nbytes = sqlite3_column_bytes(statement, col_index);
                    value = core::types::ByteVector(data, data + nbytes);
                }
                else
                {
                    value = core::types::ByteVector();
                }
                break;

            case SQLITE_NULL:
                break;
            }
        }
        return row_data;
    }

    void SQLite3::check_status(
        int code,
        std::string &&action,
        core::types::KeyValueMap &&attributes) const
    {
        if (code != SQLITE_OK)
        {
            attributes.insert_if_value("action", action);

            throw core::exception::ServiceError(
                sqlite3_errstr(code) ? sqlite3_errstr(code) : "Unknown SQLite3 failure",
                "SQLite3",                                    // service
                static_cast<core::status::Error::Code>(code), // code
                "",                                           // id
                core::status::Level::ERROR,                  // level
                core::dt::Clock::now(),                       // timepoint
                attributes);                                  // attributes
        }
    }
} // namespace core::db
