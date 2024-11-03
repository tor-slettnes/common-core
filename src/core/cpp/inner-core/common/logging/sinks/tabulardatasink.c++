/// -*- c++ -*-
//==============================================================================
/// @file tabulardatasink.c++
/// @brief Generic log sink that receives well-known data fields per contract
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tabulardatasink.h++"

namespace core::logging
{
    TabularDataSink::TabularDataSink(const std::string &sink_id,
                                     status::Level threshold,
                                     const std::optional<std::string> &contract_id,
                                     const ColumnDefaults &columns)
        : AsyncLogSink(sink_id, threshold, contract_id),
          columns_(columns)
    {
    }

    const ColumnDefaults &TabularDataSink::column_defaults() const
    {
        return this->columns_;
    }

    std::vector<std::string> TabularDataSink::column_names() const
    {
        std::vector<std::string> names;
        names.reserve(this->columns_.size());
        for (const auto &[tag, default_value] : this->column_defaults())
        {
            if (tag.has_value())
            {
                names.push_back(tag.value());
            }
        }
        names.shrink_to_fit();
        return names;
    }

    bool TabularDataSink::is_applicable(const types::Loggable &item) const
    {
        if (auto event = dynamic_cast<const status::Event *>(&item))
        {
            return (event->contract_id() == this->contract_id());
        }
        else
        {
            return false;
        }
    }

}  // namespace core::logging
