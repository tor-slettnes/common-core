/// -*- c++ -*-
//==============================================================================
/// @file tabulardatasink.h++
/// @brief Generic log sink that receives well-known data fields per contract
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "asynclogsink.h++"

#include <string>
#include <memory>
#include <ostream>

namespace core::logging
{
    using ColumnDefaults = types::TaggedValueList;

    //==========================================================================
    /// \class TabularDataSink
    /// \brief Abstract base for sinks that log a fixed set of fields

    class TabularDataSink : public AsyncLogSink
    {
        using This = TabularDataSink;
        using Super = LogSink;

    public:
        using ptr = std::shared_ptr<TabularDataSink>;

    protected:
        TabularDataSink(const std::string &sink_id,
                        status::Level threshold,
                        const std::optional<std::string> &contract_id,
                        const ColumnDefaults &columns);

    public:
        const ColumnDefaults &column_defaults() const;
        std::vector<std::string> column_names() const;

    protected:
        bool is_applicable(const types::Loggable &item) const override;

    private:
        ColumnDefaults columns_;
    };
}  // namespace core::logging
