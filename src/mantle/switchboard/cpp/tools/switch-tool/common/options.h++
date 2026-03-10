// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Options parser for switch control tool
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "argparse/command.h++"
#include "switchboard.h++"

namespace switchboard
{
    class Options : public core::argparse::CommandOptions
    {
        using Super = core::argparse::CommandOptions;

        struct SwitchControlFlags
        {
            bool clear_existing = true;
            bool invoke_interceptors = true;
            switchboard::CascadeStyle cascade_style = switchboard::CascadeStyle::DEFAULT;
            bool reenter = false;
        };

    public:
        Options(const std::string &implementation);

        void add_options() override;
        bool handle_command(const std::shared_ptr<switchboard::Provider> &provider);

    private:  // implemented in options.c++
        void add_commands();
        void get_provider();

    private:  // implemented in commands.c++
        void list_switches();
        void get_exists();
        void get_active();
        void get_inactive();
        void get_culprits();
        void get_error();
        void get_errors();
        void get_status();
        void get_localization();
        void get_activate_text();
        void get_deactivate_text();
        void get_state_text();
        void get_dependencies();
        void get_interceptors();
        void get_specs();
        void load_file();
        void save_file();
        void add_switch();
        void remove_switch();
        void clear_switches();
        void add_alias();
        void remove_alias();
        void set_localization();
        void add_dependency();
        void remove_dependency();
        void add_interceptor();
        void remove_interceptor();
        void clear_attributes();
        void set_attributes();
        void set_target();
        void set_active();
        void set_inactive();
        void set_auto();
        void set_error();

        void on_monitor_start() override;
        void on_monitor_end() override;

        static void on_signal_spec(core::signal::MappingAction action,
                                   const switchboard::SwitchName &name,
                                   const switchboard::Specification &spec);

        static void on_signal_status(core::signal::MappingAction action,
                                     const switchboard::SwitchName &name,
                                     const switchboard::Status &status);

        std::vector<switchboard::SwitchRef> get_switches_or_all();
        switchboard::SwitchRef get_switch(bool required);
        SwitchControlFlags get_switch_control_flags();
        switchboard::StateSet get_states();
        void print_states() const;
        void print_tvlist(
            const core::types::TaggedValueList &tvlist,
            const std::unordered_set<std::string> &selection = {},
            std::size_t alignment_column = 16) const;

    public:
        std::shared_ptr<switchboard::Provider> provider;

    private:
        const std::string implementation;
        std::string signal_handle;
        bool verbose;
    };

    extern std::unique_ptr<Options> options;
}  // namespace switchboard
