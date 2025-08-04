/// -*- c++ -*-
//==============================================================================
/// @file multilogger-sqlite3-sink.c++
/// @brief Log tabular data to SQLite3 database
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-sqlite3-sink.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"
#include "string/misc.h++"

namespace multilogger
{

    //--------------------------------------------------------------------------
    // SQLiteSink

    SQLiteSink::SQLiteSink(const std::string &sink_id)
        : Super(sink_id),
          TabularData(),
          RotatingPath(sink_id, ".db"),
          table_name_(DEFAULT_TABLE_NAME),
          batch_size_(DEFAULT_BATCH_SIZE),
          batch_timeout_(std::chrono::seconds(DEFAULT_BATCH_TIMEOUT))
    {
    }

    SQLiteSink::~SQLiteSink()
    {
        this->flush();
    }

    void SQLiteSink::load_settings(const core::types::KeyValueMap &settings)
    {
        Super::load_settings(settings);
        this->load_columns(settings);
        this->load_rotation(settings);
        this->load_db_settings(settings);
        this->load_level_map(settings);
    }

    void SQLiteSink::load_db_settings(const core::types::KeyValueMap &settings)
    {
        if (const core::types::Value &value = settings.get(SETTING_TABLE_NAME))
        {
            this->set_table_name(value.as_string());
        }

        if (const core::types::Value &value = settings.get(SETTING_BATCH_SIZE))
        {
            this->set_batch_size(value.as_uint());
        }

        if (const core::types::Value &value = settings.get(SETTING_BATCH_TIMEOUT))
        {
            this->set_batch_timeout(std::chrono::seconds(value.as_uint()));
        }
    }

    std::string SQLiteSink::table_name() const
    {
        return this->table_name_;
    }

    void SQLiteSink::set_table_name(const std::string &name)
    {
        this->table_name_ = name;
    }

    std::size_t SQLiteSink::batch_size() const
    {
        return this->batch_size_;
    }

    void SQLiteSink::set_batch_size(const std::size_t &size)
    {
        this->batch_size_ = size;
    }

    core::dt::Duration SQLiteSink::batch_timeout() const
    {
        return this->batch_timeout_;
    }

    void SQLiteSink::set_batch_timeout(const core::dt::Duration &timeout)
    {
        this->batch_timeout_ = timeout;
    }

    void SQLiteSink::open()
    {
        core::dt::TimePoint last_aligned = core::dt::last_aligned(
            core::dt::Clock::now(),
            this->rotation_interval(),
            this->use_local_time());

        this->open_file(last_aligned);
        Super::open();
    }

    void SQLiteSink::close()
    {
        Super::close();
        this->close_file();
    }

    void SQLiteSink::open_file(const core::dt::TimePoint &tp)
    {
        RotatingPath::open_file(tp);
        try
        {
            this->db.open(this->current_path());
            this->create_table();
        }
        catch (...)
        {
            logf_warning("Failed to open log sink %r output file %r: %r",
                         this->sink_id(),
                         this->current_path(),
                         std::current_exception());
        }
    }

    void SQLiteSink::close_file()
    {
        RotatingPath::close_file();
        this->db.close();
    }

    void SQLiteSink::create_table()
    {
        std::vector<core::db::SQLite3::ColumnSpec> db_columns;
        db_columns.reserve(this->columns().size());
        for (const auto &log_column : this->columns())
        {
            db_columns.push_back({
                .name = log_column.column_name.value_or(log_column.field_name),
                .type = log_column.column_type,
            });
        }

        this->db.create_table(this->table_name(), db_columns);
    }

    void SQLiteSink::create_placeholders()
    {
        this->placeholders = this->db.get_placeholders(this->table_name());
    }

    bool SQLiteSink::handle_item(const core::types::Loggable::ptr &item)
    {
        this->check_rotation(item->timepoint());
        this->pending_rows.push_back(this->row_data(item, this->use_local_time()));
        return true;
    }

    void SQLiteSink::worker()
    {
        this->pending_rows.reserve(this->batch_size());
        this->create_placeholders();

        while (this->is_open())
        {
            bool flush = true;
            if (auto opt_item = this->queue()->get(this->batch_timeout()))
            {
                this->try_handle_item(opt_item.value());
                flush = (this->pending_rows.size() >= this->batch_size());
            }

            if (flush)
            {
                this->flush();
            }
        }
    }

    void SQLiteSink::flush()
    {
        std::scoped_lock lck(this->rows_lock_);

        if (this->pending_rows.size())
        {
            try
            {
                this->db.insert_multi(this->table_name(), this->pending_rows);
            }
            catch (...)
            {
                logf_warning("Log sink %r failed to flush %d messages to %r: %s",
                             this->sink_id(),
                             this->pending_rows.size(),
                             this->current_path(),
                             std::current_exception());
            }
            this->pending_rows.clear();
        }
    }
}  // namespace multilogger
