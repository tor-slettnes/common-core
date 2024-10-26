// -*- c++ -*-
//==============================================================================
/// @file logger-base.h++
/// @brief Logging service - abstract base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-types.h++"
#include "status/event.h++"

namespace logger
{
    class BaseLogger
    {
    protected:
        BaseLogger(const std::string &identity);

    public:
        const std::string &identity() const;

        virtual void log(
            core::status::Event::ptr event) = 0;

        virtual void add_contract(
            const std::string &contract_id,
            const ContractTemplate &contract_template) = 0;

        virtual void remove_contract(
            const std::string &contract_id) = 0;

        virtual ContractTemplate get_contract(
            const std::string &contract_id) const = 0;

        virtual ContractsMap list_contracts() const = 0;

        virtual std::shared_ptr<EventQueue> listen(
            const Filter &filter) = 0;

    private:
        std::string identity_;
    };
}  // namespace logger
