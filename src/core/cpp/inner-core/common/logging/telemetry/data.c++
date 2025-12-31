/// -*- c++ -*-
//==============================================================================
/// @file data.c++
/// @brief A loggable item comprising arbitrary key/value data
/// @author Tor Slettnes
//==============================================================================

#include "data.h++"

namespace core::logging
{
    Data::Data(const ContractID &contract_id,
               const dt::TimePoint &tp,
               const types::KeyValueMap &attributes)
        : types::Loggable(tp, attributes),
          contract_id_(contract_id)
    {
    }

    // Copy constructor to ensure we obtain values from derived classes
    Data::Data(const Data &other)
    {
        *this = other;
    }

    Data &Data::operator=(Data &&other) noexcept
    {
        Super::operator=(other);
        std::swap(this->contract_id_, other.contract_id_);
        return *this;
    }

    Data &Data::operator=(const Data &other) noexcept
    {
        Super::operator=(other);
        this->contract_id_ = other.contract_id();
        return *this;
    }

    bool Data::operator==(const Data &other) const noexcept
    {
        return Super::operator==(other) &&
               (this->contract_id() == other.contract_id());
    }

    Data::ContractID Data::contract_id() const noexcept
    {
        return this->contract_id_;
    }

    std::string Data::class_name() const noexcept
    {
        return "Data";
    }

    std::vector<std::string> Data::data_fields() noexcept
    {
        std::vector<std::string> fields = Loggable::loggable_fields();
        fields.insert(
            fields.begin(),
            {

                This::FIELD_CONTRACT_ID,
            });
        return fields;
    }

    std::vector<std::string> Data::field_names() const noexcept
    {
        return This::data_fields();
    }

    types::Value Data::get_field_as_value(const std::string &field_name) const
    {
        if (field_name == This::FIELD_CONTRACT_ID)
        {
            return this->contract_id();
        }
        else
        {
            return Super::get_field_as_value(field_name);
        }
    }

}  // namespace core::logging
