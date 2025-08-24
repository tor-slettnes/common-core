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

        enum class SortDirection
        {
            ASCENDING,
            DESCENDING,
        };

        virtual std::vector<std::string> column_names(
            const std::string &table_name) const = 0;

        virtual std::size_t column_count(
            const std::string &table_name) const = 0;

        static std::string quote_literal(const std::string &literal);
        static std::string quote_ident(const std::string &identifier);

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
