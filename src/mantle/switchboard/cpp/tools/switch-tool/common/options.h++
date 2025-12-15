// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Options parser for switch control tool
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "argparse/command.h++"
#include "switchboard.h++"

namespace switchboard
{
    class Options : public core::argparse::CommandOptions
    {
        using Super = core::argparse::CommandOptions;

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
        void get_state_text();
        void get_target_text();
        void get_dependencies();
        void get_interceptors();
        void get_specs();
        void load_file();
        void add_switch();
        void remove_switch();
        void set_localization();
        void add_dependency();
        void remove_dependency();
        void add_interceptor();
        void remove_interceptor();
        // void clear_attributes();
        // void set_attributes();
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

        switchboard::SwitchRef get_switch(bool required);
        switchboard::StateMask get_states();
        void print_states();

    public:
        std::shared_ptr<switchboard::Provider> provider;

    private:
        const std::string implementation;
        std::string signal_handle;
    };

    extern std::unique_ptr<Options> options;
}  // namespace switchboard
