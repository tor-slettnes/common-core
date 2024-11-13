/// -*- c++ -*-
//==============================================================================
/// @file sqlcommands.h++
/// @brief Basic SQL command builder
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"
#include "types/streamable.h++"

#include <ostream>
#include <list>

namespace core::sql
{
    // using core::types::ValueType;
    // core::db::create_table("fortyfive").columns({
    //     {.name = "one", .type = ValueType::SINT},
    //     {.name = "two", .type = ValueType::STRING},
    // });
    // // core::db::Command().SELECT("*").FROM("fortyfive").WHERE("ab == 3").AND();

    // core::db::Command mycommand;
    // mycommand.CREATE_TABLE("mytable", {
    //    {"firstcolumn", ValueType::SINT},
    // }

    class Command : public core::types::Streamable
    {
        using This = Command;

    protected:
        using ValueType = core::types::ValueType;

    public:
        struct ColumnSpec
        {
            std::string name;
            ValueType type = ValueType::NONE;
            bool primary = false;
        };
        using ColumnSpecs = std::vector<ColumnSpec>;

    public:
        virtual Command &CREATE_TABLE(
            const std::string &table_name,
            const ColumnSpecs &columns,
            const ValueType &fallback_type=ValueType::STRING);

        virtual Command &SELECT(const std::vector<std::string> &columns = {});
        virtual Command &FROM(const std::string &table);
        virtual Command &WHERE(const std::string &condition);
        virtual Command &IF(const std::string &condition);

    public:
        // Command &add(const std::string &string);
        // Command &add(const std::vector<std::string> &string);

        void to_stream(std::ostream &stream) const override;

    private:
        Command &add(const std::string &string);
        Command &add_column_spec(
            const ColumnSpecs &columns,
            const ValueType &fallback_type);

        Command &add_column_selection(
            const std::vector<std::string> &columns);


    private:
        static core::types::SymbolMap<ValueType> type_names;
        std::stringstream command;
    };
}  // namespace core::db
