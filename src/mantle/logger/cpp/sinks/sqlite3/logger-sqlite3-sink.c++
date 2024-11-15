/// -*- c++ -*-
//==============================================================================
/// @file sqlitesink.h++
/// @brief Log tabular data to SQLite3 database
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-sqlite3-sink.h++"
#include "logging/message/message.h++"

namespace logger
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
            ;
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

    const core::types::KeyValueMap &SQLiteSink::level_map() const
    {
        return this->level_map_;
    }

    void SQLiteSink::set_level_map(const core::types::KeyValueMap &level_map)
    {
        this->level_map_ = level_map;
    }


    std::optional<core::logging::ColumnSpec> SQLiteSink::column_spec(const core::types::Value &column_data)
    {
        if (const core::types::ValueListPtr colspec = column_data.get_valuelist())
        {
            return core::logging::ColumnSpec({
                .event_field = colspec->get(0).as_string(),
                .column_name = colspec->get(1).as_string(),
                .column_type = colspec->get_as<core::logging::ColumnType>(2)
                                   .value_or(core::logging::ColumnType::TEXT),
                .format_string = colspec->get(3).as_string(),
            });
        }
        else if (column_data.is_string())
        {
            return core::logging::ColumnSpec({
                .event_field = column_data.as_string(),
            });
        }
        else
        {
            return {};
        }
    }

    void SQLiteSink::open()
    {
        this->open_file(this->last_aligned(core::dt::Clock::now()));
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
        std::stringstream header;
        std::stringstream placeholders;
        std::string delimiter;

        placeholders << "(";
        for (const core::logging::ColumnSpec &spec : this->columns())
        {
            header << delimiter
                   << std::quoted(!spec.column_name.empty()
                                      ? spec.column_name
                                      : spec.event_field)
                   << " "
                   << spec.column_type;

            placeholders << delimiter
                         << "?";

            delimiter = ", ";
        }
        placeholders << ")";

        this->db.execute(core::str::format(
            "CREATE TABLE IF NOT EXISTS %s (%s)",
            this->table_name(),
            header.rdbuf()));

        this->placeholders = placeholders.str();
    }

    void SQLiteSink::worker()
    {
        std::size_t pending_count = 0;
        this->pending_rows.reserve(this->batch_size());

        while (!this->queue.closed())
        {
            bool flush = false;

            if (auto opt_event = pending_count
                                     ? this->queue.get(this->batch_timeout())
                                     : this->queue.get())
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
    }

    void SQLiteSink::capture_event(const core::status::Event::ptr &event)
    {
        this->check_rotation(event->timepoint());
        core::types::KeyValueMap kvmap = event->as_kvmap(this->level_map());
        core::types::ValueList row;

        for (const core::logging::ColumnSpec &spec : this->columns())
        {
            const core::types::Value &value = kvmap.get(spec.event_field);
            row.push_back(!spec.format_string.empty()
                              ? core::str::format(spec.format_string, value)
                              : value);
        }
        this->pending_rows.push_back(row);
    }

    void SQLiteSink::flush_events()
    {
        this->db.execute_multi(
            core::str::format("INSERT INTO %s VALUES %s",
                              this->table_name(),
                              this->placeholders),
            this->pending_rows);

        this->pending_rows.clear();
    }

}  // namespace logger
