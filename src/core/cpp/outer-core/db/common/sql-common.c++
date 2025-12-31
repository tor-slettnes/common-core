/// -*- c++ -*-
//==============================================================================
/// @file sql-common.c++
/// @brief Common functionality for SQL-based database wrappers
/// @author Tor Slettnes
//==============================================================================

#include "sql-common.h++"
#include "string/misc.h++"
#include <sstream>

namespace core::db
{
    std::string SQL::select_query(
        const std::vector<std::string> &columns,
        const std::string &table,
        const std::vector<std::string> &conditions,
        const std::string &order_by,
        SortDirection direction,
        uint limit) const
    {
        std::stringstream sql;
        sql << "SELECT ";
        core::str::join(sql, columns.begin(), columns.end(), ", ");

        sql << " FROM " << SQL::quote_ident(table);

        if (!conditions.empty())
        {
            sql << " WHERE " << core::str::join(conditions, " AND ");
        }

        if (!order_by.empty())
        {
            sql << " ORDER BY "
                << order_by
                << (direction == SortDirection::ASCENDING ? " ASC" : " DESC");
        }

        if (limit)
        {
            sql << " LIMIT " << limit;
        }

        return sql.str();
    }


    std::string SQL::quote_literal(const std::string &literal)
    {
        return "'" + literal + "'";
    }

    std::string SQL::quote_ident(const std::string &identifier)
    {
        return "\"" + identifier + "\"";
    }

    const SQL::ColumnNames SQL::ALL_COLUMNS = {"*"};

}  // namespace core::db
