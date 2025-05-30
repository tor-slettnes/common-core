/// -*- c++ -*-
//==============================================================================
/// @file sink.c++
/// @brief Data capture sink - abstract backend
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sink.h++"
#include "settings/settings.h++"

#include <iostream>

namespace core::logging
{
    const std::string SETTING_CONTRACT_ID = "contract_id";
    const std::string SETTING_THRESHOLD = "threshold";
    const status::Level DEFAULT_THRESHOLD = status::Level::NONE;

    //--------------------------------------------------------------------------
    /// @class Sink

    Sink::Sink(const SinkID &sink_id,
               const std::optional<Loggable::ContractID> &contract_id)
        : is_open_(false),
          sink_id_(sink_id),
          contract_id_(contract_id),
          threshold_(DEFAULT_THRESHOLD)
    {
    }

    void Sink::load_settings(const types::KeyValueMap &settings)
    {
        if (const core::types::Value &contract_id = settings.get(SETTING_CONTRACT_ID))
        {
            this->set_contract_id(contract_id.as_string());
        }

        if (auto threshold = settings.try_get_as<status::Level>(SETTING_THRESHOLD))
        {
            this->set_threshold(threshold.value());
        }
    }

    SinkID Sink::sink_id() const
    {
        return this->sink_id_;
    }

    SinkType Sink::sink_type() const
    {
        return this->sink_type_;
    }

    void Sink::set_sink_type(const SinkType &sink_type)
    {
        this->sink_type_ = sink_type;
    }

    void Sink::set_contract_id(const std::optional<Loggable::ContractID> &contract_id)
    {
        this->contract_id_ = contract_id;
    }

    std::optional<Loggable::ContractID> Sink::contract_id() const
    {
        return this->contract_id_;
    }

    void Sink::set_threshold(status::Level threshold)
    {
        this->threshold_ = threshold;
    }

    status::Level Sink::threshold() const
    {
        return this->threshold_;
    }

    bool Sink::is_applicable(const types::Loggable &loggable) const
    {
        if (this->contract_id() &&
            (this->contract_id().value() != loggable.contract_id()))
        {
            return false;
        }

        if (auto *event = dynamic_cast<const status::Event *>(&loggable))
        {
            if (event->level() < this->threshold())
            {
                return false;
            }
        }

        return true;
    }

    bool Sink::is_open() const
    {
        return this->is_open_;
    }

    void Sink::open()
    {
        this->is_open_ = true;
    }

    void Sink::close()
    {
        this->is_open_ = false;
    }

    bool Sink::capture(const types::Loggable::ptr &loggable)
    {
        if (this->is_open() && this->is_applicable(*loggable))
        {
            return this->try_handle_item(loggable);
        }
        else
        {
            return false;
        }
    }

    bool Sink::try_handle_item(const types::Loggable::ptr &loggable)
    {
        try
        {
            return this->handle_item(loggable);
        }
        catch (...)
        {
            this->close();
            return false;
        }
    }

}  // namespace core::logging
