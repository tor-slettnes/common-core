/// -*- c++ -*-
//==============================================================================
/// @file multilogger-sqlite3-sink.c++
/// @brief Log tabular data to SQLite3 database
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-sqlite3-sink.h++"
#include "logging/message/message.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"
#include "string/misc.h++"

namespace multilogger
{

    //--------------------------------------------------------------------------
    // SQLiteSink

    SQLiteSink::SQLiteSink(const std::string &sink_id)
        : AsyncLogSink(sink_id),
          TabularData(),
          RotatingPath(sink_id, ".db"),
          table_name_(DEFAULT_TABLE_NAME),
          batch_size_(DEFAULT_BATCH_SIZE),
          batch_timeout_(std::chrono::seconds(DEFAULT_BATCH_TIMEOUT))
    {
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
        AsyncLogSink::open();
    }

    void SQLiteSink::close()
    {
        AsyncLogSink::close();
        this->close_file();
    }

    void SQLiteSink::open_file(const core::dt::TimePoint &tp)
    {
        RotatingPath::open_file(tp);
        this->db.open(this->current_path());
        this->create_table();
    }

    void SQLiteSink::close_file()
    {
        this->db.close();
        RotatingPath::close_file();
    }

    void SQLiteSink::create_table()
    {
        std::stringstream sql;
        std::string delimiter;

        sql << "CREATE TABLE IF NOT EXISTS "
            << std::quoted(this->table_name())
            << " (";

        for (const core::logging::ColumnSpec &spec : this->columns())
        {
            sql << delimiter
                << std::quoted(spec.column_name.value_or(spec.event_field));

            if (auto type_name = core::logging::column_type_names.to_string(spec.column_type))
            {
                sql << " " << *type_name;
            }

            delimiter = ", ";
        }

        sql << ")";
        this->db.execute(sql.str());
    }

    void SQLiteSink::create_placeholders()
    {
        std::vector<std::string> placeholders(this->columns().size(), "?");
        this->placeholders = "(" + core::str::join(placeholders, ", ") + ")";
    }

    void SQLiteSink::worker()
    {
        std::size_t pending_count = 0;
        this->pending_rows.reserve(this->batch_size());
        this->create_placeholders();

        while (!this->queue.closed())
        {
            try
            {
                bool flush = false;
                if (auto opt_event = this->queue.get(this->batch_timeout()))
                {
                    this->capture_event(opt_event.value());
                    flush = (++pending_count >= this->batch_size());
                }
                else
                {
                    flush = (pending_count > 0);
                }

                if (flush)
                {
                    this->flush_events();
                    pending_count = 0;
                }
            }
            catch (...)
            {
                this->queue.close();
                logf_error("Log sink %r failed to capture event: %s",
                           this->sink_id(),
                           std::current_exception());
            }
        }
    }

    void SQLiteSink::capture_event(const core::status::Event::ptr &event)
    {
        this->check_rotation(event->timepoint());
        this->pending_rows.push_back(this->row_data(event, this->use_local_time()));
    }

    void SQLiteSink::flush_events()
    {
        std::stringstream cmd;
        cmd << "INSERT INTO "
            << std::quoted(this->table_name())
            << " VALUES "
            << this->placeholders;

        this->db.execute_multi(cmd.str(), this->pending_rows);

        // this->db.execute_multi(
        //     core::str::format("INSERT INTO %s VALUES %s",
        //                       this->table_name(),
        //                       this->placeholders),
        //     this->pending_rows);

        this->pending_rows.clear();
    }

}  // namespace multilogger
