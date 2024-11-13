/// -*- c++ -*-
//==============================================================================
/// @file sqlcommands.c++
/// @brief Basic SQL command builder
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sqlcommand.h++"

namespace core::sql
{

    Command &Command::CREATE_TABLE(
        const std::string &table_name,
        const ColumnSpecs &columns,
        const ValueType &fallback_type)
    {
        this->add("CREATE TABLE");
        this->add(str::quoted(table_name));
        this->add_column_spec(columns, fallback_type);
        return *this;
    }

    Command &Command::SELECT(const std::vector<std::string> &columns)
    {
        this->add("SELECT");
        this->add_column_selection(columns);
        return *this;
    }

    Command &Command::FROM(const std::string &table)
    {
        this->add("FROM");
        this->add(str::quoted(table));
        return *this;
    }

    Command &Command::WHERE(const std::string &condition)
    {
        this->add("WHERE (");
        this->add(condition);
        this->add(")");
        return *this;
    }

    Command &Command::IF(const std::string &condition)
    {
        this->add("IF");
        this->add(condition);
        return *this;
    }

    Command &Command::add(const std::string &string)
    {
        if ((this->command.tellp() > 0) && !string.empty())
        {
            this->command << " ";
        }
        this->command << string;
        return *this;
    }

    Command &Command::add_column_spec(
        const ColumnSpecs &columns,
        const ValueType &fallback_type)
    {
        std::string fallback_type_name = This::type_names.at(fallback_type);
        std::string delimiter;

        this->add("(");
        for (const ColumnSpec &spec : columns)
        {
            this->add(delimiter);
            delimiter = ", ";

            this->add(spec.name);
            this->add(This::type_names.to_string(spec.type, fallback_type_name));

            if (spec.primary)
            {
                this->add("PRIMARY KEY");
            }
        }
        this->add(")");
        return *this;
    }

    Command &Command::add_column_selection(
        const std::vector<std::string> &columns)
    {
        if (!columns.empty())
        {
            this->add("(");
            for (const std::string &column : columns)
            {
                this->add(str::quoted(column));
            }
            this->add(")");
        }
        return *this;
    }

    core::types::SymbolMap<Command::ValueType> Command::type_names = {
        {ValueType::NONE, ""},
        {ValueType::UINT, "INT"},
        {ValueType::SINT, "INT"},
        {ValueType::REAL, "REAL"},
        {ValueType::STRING, "TEXT"},
        {ValueType::BYTEVECTOR, "BLOB"},
    };

}  // namespace core::db
