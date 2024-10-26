// -*- c++ -*-
//==============================================================================
/// @file logger-native.h++
/// @brief Logging service - host native interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-base.h++"
#include "logging/dispatchers/async-dispatcher.h++"

namespace logger
{
    class NativeLogger : public BaseLogger
    {
        using This = NativeLogger;
        using Super = BaseLogger;

    protected:
        NativeLogger(const std::string &identity);

    public:
        void log(
            core::status::Event::ptr event) override;

        void add_contract(
            const std::string &contract_id,
            const ContractTemplate &contract_template) override;

        void remove_contract(
            const std::string &contract_id) override;

        ContractTemplate get_contract(
            const std::string &contract_id) const override;

        ContractsMap list_contracts() const override;

        std::shared_ptr<EventQueue> listen(
            const Filter &filter) override;
    };
}  // namespace logger
