/// -*- c++ -*-
//==============================================================================
/// @file data.h++
/// @brief A loggable item comprising arbitrary key/value data
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/loggable.h++"

namespace core::logging
{
    class Data : public types::Loggable
    {
        using This = Data;
        using Super = types::Loggable;

    public:
        using ptr = std::shared_ptr<Data>;
        inline static const std::string FIELD_CONTRACT_ID = "contract_id";

    public:
        Data(const ContractID &contract_id = {},
             const dt::TimePoint &tp = {},
             const types::KeyValueMap &attributes = {});

        // Copy constructor to ensure we obtain values from derived classes
        Data(const Data &other);

    public:
        Data &operator=(Data &&other) noexcept;
        Data &operator=(const Data &other) noexcept;
        bool operator==(const Data &other) const noexcept;

        ContractID contract_id() const noexcept override;

    protected:
        std::string class_name() const noexcept override;

    public:
        static std::vector<std::string> data_fields() noexcept;
        std::vector<std::string> field_names() const noexcept override;
        types::Value get_field_as_value(const std::string &field_name) const override;

    protected:
        ContractID contract_id_;
    };

}  // namespace core::logging
