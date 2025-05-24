/// -*- c++ -*-
//==============================================================================
/// @file sink.h++
/// @brief Data capture sink - abstract backend
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "status/level.h++"
#include "types/loggable.h++"
#include "types/value.h++"
#include "types/valuemap.h++"

#include <memory>
#include <string>

namespace core::logging
{
    using types::Loggable;
    using SinkID = std::string;
    using SinkType = std::string;


    //--------------------------------------------------------------------------
    // Constants

    constexpr auto SETTING_LOG_SINKS = "log sinks";

    //--------------------------------------------------------------------------
    /// @class Sink
    /// @brief Abstract data receiver

    class Sink
    {
        friend class SinkFactory;
        using This = Sink;

    public:
        using ptr = std::shared_ptr<Sink>;

    protected:
        Sink(const SinkID &sink_id,
             const std::optional<Loggable::ContractID> &contract_id = {});

        virtual ~Sink() {}

    public:
        SinkID sink_id() const;
        SinkType sink_type() const;

    protected:
        void set_sink_type(const SinkType &sink_type);

    public:
        virtual void load_settings(const types::KeyValueMap &settings);

        void set_contract_id(const std::optional<Loggable::ContractID> &contract_id);
        std::optional<Loggable::ContractID> contract_id() const;

        virtual void set_threshold(status::Level threshold);
        status::Level threshold() const;

    public:
        virtual bool is_applicable(const types::Loggable &loggable) const;
        virtual bool is_open() const;
        virtual void open();
        virtual void close();
        virtual bool capture(const types::Loggable::ptr &loggable);

    protected:
        virtual bool try_handle_item(const types::Loggable::ptr &loggable);
        virtual bool handle_item(const types::Loggable::ptr &loggable) = 0;

    private:
        bool is_open_;
        SinkID sink_id_;
        SinkType sink_type_;
        std::optional<Loggable::ContractID> contract_id_;
        status::Level threshold_;
    };
}  // namespace core::logging
