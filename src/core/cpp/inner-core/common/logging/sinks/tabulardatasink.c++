/// -*- c++ -*-
//==============================================================================
/// @file tabulardatasink.c++
/// @brief Generic log sink that receives well-known data fields per contract
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tabulardatasink.h++"

namespace core::logging
{
    TabularDataSink::TabularDataSink(const std::string contract_id,
                                     const ColumnDefaults &columns)
        : contract_id_(contract_id),
          columns_(columns)
    {
    }

    std::string TabularDataSink::contract_id() const
    {
        return this->contract_id_;
    }

    ColumnDefaults TabularDataSink::column_defaults() const
    {
        return this->columns_;
    }

    std::vector<std::string> TabularDataSink::column_names() const
    {
        std::vector<std::string> names;
        names.reserve(this->columns_.size());
        for (uint c = 0; c < this->columns_.size(); c++)
        {
            const auto &[tag, value] = this->columns_.at(c);
            names.push_back(tag.value_or(""));
        }
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
