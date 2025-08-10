/// -*- c++ -*-
//==============================================================================
/// @file sql-common.h++
/// @brief Common functionality for SQL-based database wrappers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"
#include "thread/blockingqueue.h++"

namespace core::db
{
    class SQL
    {
    public:
        using ColumnName = std::string;
        using ColumnNames = std::vector<ColumnName>;
        using RowData = core::types::ValueList;
        using MultiRowData = std::vector<RowData>;
        using QueryCallbackFunction = std::function<bool(core::types::TaggedValueList &&)>;
        using QueryResponseQueue = core::types::BlockingQueue<core::types::TaggedValueList>;

        enum class SortDirection
        {
            ASCENDING,
            DESCENDING,
        };

        virtual std::vector<std::string> column_names(
            const std::string &table_name) const = 0;

        virtual std::size_t column_count(
            const std::string &table_name) const = 0;

        std::string select_query(
            const ColumnNames &columns,
            const std::string &table,
            const std::vector<std::string> &conditions = {},
            const std::string &order_by = {},
            SortDirection direction = SortDirection::ASCENDING,
            uint limit = 0) const;

    public:
        static const ColumnNames ALL_COLUMNS;
    };
}  // namespace core::db
