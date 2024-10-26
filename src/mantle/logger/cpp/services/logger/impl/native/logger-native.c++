// -*- c++ -*-
//==============================================================================
/// @file logger-native.c++
/// @brief Logging service - host native implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-native.h++"
#include "logging/logging.h++"

namespace logger
{
    NativeLogger::NativeLogger(const std::string &identity)
        : Super(identity)
    {
    }

    void NativeLogger::log(
        core::status::Event::ptr event)
    {
        core::logging::message_dispatcher.submit(event);
    }

    void NativeLogger::add_contract(
        const std::string &contract_id,
        const ContractTemplate &contract_template)
    {
    }

    void NativeLogger::remove_contract(
        const std::string &contract_id)
    {
    }

    ContractTemplate NativeLogger::get_contract(
        const std::string &contract_id) const
    {
        return {};
    }

    ContractsMap NativeLogger::list_contracts() const
    {
        return {};
    }

    std::shared_ptr<EventQueue> NativeLogger::listen(
        const Filter &filter)
    {
        return {};
    }

}  // namespace logger
