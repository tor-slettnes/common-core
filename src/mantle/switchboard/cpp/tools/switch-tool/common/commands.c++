// -*- c++ -*-
//==============================================================================
/// @file commands.c++
/// @brief Options parser for switch control tool
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "switchboard.h++"
#include "status/exceptions.h++"
#include "chrono/date-time.h++"

#include <algorithm>

namespace cc::platform::switchboard
{
    void Options::add_commands()
    {
        this->add_command(
            "provider",
            {},
            "Return a boolean indicating whether a switchboard provider is "
            "available/connected (i.e., the switchboard service is running).",
            std::bind(&Options::get_provider, this));

        this->add_command(
            "list-states",
            {},
            "List available switch states and exit.",
            std::bind(&Options::print_states, this));

        this->add_command(
            "list",
            {"[state]", "[field ...]"},
            "Return a list of available switches. "
            "Optionally include the current state of each switch next to "
            "its name, and/or additional specification and/or status fields "
            "on separate lines.  Use the \"--verbose\" option to include all "
            "specification and status fields",
            std::bind(&Options::list_switches, this));

        this->add_command(
            "exists",
            {"SWITCH"},
            "Return a status indicating whether if the switch exists. "
            "See the \"--status\" option above.",
            std::bind(&Options::get_exists, this));

        this->add_command(
            "active",
            {"SWITCH"},
            "Return a status indicating whether if the switch is active. "
            "See the \"--status\" option above.",
            std::bind(&Options::get_active, this));

        this->add_command(
            "inactive",
            {"SWITCH"},
            "Return a status indicating whether if the switch is not active. "
            "See the \"--status\" option above.",
            std::bind(&Options::get_inactive, this));

        this->add_command(
            "culprits",
            {"SWITCH", "[active|inactive]"},
            "Print a list of switches that prevent SWITCH from being either "
            "active (default) or inactive. This list may be empty (if SWITCH "
            "is already in the specified state) or may be just SWITCH itself "
            "(if SWITCH is not in the speciifed state but would be based on "
            "the current value of its predecessors, which normally indicates "
            "a manual dependency or override). "
            "Use the \"--verbose\"  to also print the current state and any "
            "error message for each culprit.",
            std::bind(&Options::get_culprits, this));

        this->add_command(
            "error",
            {"SWITCH"},
            "Print any error directly owned by SWITCH. (This will be empty if its "
            "currently settled state is not STATE_FAILED).",
            std::bind(&Options::get_error, this));

        this->add_command(
            "errors",
            {"SWITCH"},
            "Print any error owned by SWITCH as well those of its culprits. "
            "Each output line contains the name of the owning switch and the "
            "corresponding error.",
            std::bind(&Options::get_errors, this));

        this->add_command(
            "status",
            {"[PATTERN ...]"},
            "Print the full status for switches mathing any provided PATTERN. "
            "Each pattern may contain shell-style globbing characters such as "
            "'*' or '?'. If no pattern is provided, list the status of all "
            "switches.",
            std::bind(&Options::get_status, this));

        this->add_command(
            "localization",
            {"SWITCH"},
            "Print SWITCH's localization",
            std::bind(&Options::get_localization, this));

        this->add_command(
            "state_text",
            {"SWITCH", "STATE"},
            "Print localization of the specified STATE of SWITCH.",
            std::bind(&Options::get_state_text, this));

        this->add_command(
            "activate_text",
            {"SWITCH"},
            "Print text describing SWITCH's transition to ACTIVE.",
            std::bind(&Options::get_activate_text, this));

        this->add_command(
            "deactivate_text",
            {"SWITCH"},
            "Print text describing SWITCH's transition to INACTIVE.",
            std::bind(&Options::get_deactivate_text, this));

        this->add_command(
            "dependencies",
            {"[PATTERN ...]"},
            "Print the dependencies of switches mathing any provided PATTERN. "
            "Each pattern may contain shell-style globbing characters such as "
            "'*' or '?'. If no pattern is provided, list dependencies of all "
            "all switches.  By default only the name of each predecessor is "
            "printed; use \"--verbose\" to show more details.",
            std::bind(&Options::get_dependencies, this));

        this->add_command(
            "interceptors",
            {"[PATTERN ...]"},
            "Print interceptors for switches mathing any provided PATTERN. "
            "Each pattern may contain shell-style globbing characters such as "
            "'*' or '?'. If no pattern is provided, list dependencies of all "
            "all switches.  By default only the name of each interceptor is "
            "printed; use \"--verbose\" to show more details.",
            std::bind(&Options::get_interceptors, this));

        this->add_command(
            "specification",
            {"[PATTERN ...]"},
            "Print the full specifcation for switches mathing any provided "
            "PATTERN. Each pattern may contain shell-style globbing characters "
            "such as '*' or '?'. If no pattern is provided, list specifications "
            "for all switches.",
            std::bind(&Options::get_specs, this));

        this->add_command(
            "spec",
            {"[PATTERN ...]"},
            "Alias for \"specification\".",
            std::bind(&Options::get_specs, this));

        this->add_command(
            "load",
            {"FILENAME[.json]"},
            "Load switches from the specified JSON file. A '.json' suffix is "
            "added if missing. The file is opened in this client, and unless "
            "its path is absolute, is is expected in the standard configuration "
            "folder(s).",
            std::bind(&Options::load_file, this));

        this->add_command(
            "save",
            {"FILENAME[.json]"},
            "Save switches from to specified JSON file. A '.json' suffix is "
            "added if missing. The file is created in this client, and unless "
            "its path is absolute, will be written to the machine-specific "
            "folder folderd(s).",
            std::bind(&Options::save_file, this));

        this->add_command(
            "add",
            {"SWITCH"},
            "Add or update a switch, with in initial position ACTIVE (default: \"off\")",
            std::bind(&Options::add_switch, this));

        this->add_command(
            "remove",
            {"SWITCH"},
            "Remove an existing switch",
            std::bind(&Options::remove_switch, this));

        this->add_command(
            "clear",
            {"[reload]"},
            "Clear all switches, optionally reloading defaults",
            std::bind(&Options::clear_switches, this));

        this->add_command(
            "add_alias",
            {"SWITCH", "ALIAS", "..."},
            "Add one or more aliases for a switch",
            std::bind(&Options::add_alias, this));

        this->add_command(
            "remove_alias",
            {"SWITCH", "ALIAS", "..."},
            "Remove one or more aliases for a switch",
            std::bind(&Options::remove_alias, this));

        this->add_command(
            "set_localization",
            {"SWITCH", "LANGAUGE", "[{text|on|off|activating|active|deactivating|inactive|failing|failed} VALUE] ..."},
            "Change the localization of a switch in the specified language",
            std::bind(&Options::set_localization, this));

        this->add_command(
            "add_dependency",
            {"SWITCH", "PREDECESSOR", "[positive|negative|toggle]", "[hard]",
             "[sufficient]"
             "[manual | STATE[,...]]"},
            "Add a dependency on PREDECESSOR to SWITCH, with optional criteria:\n"
            "\n"
            "* \"positive\" means that the dependency is satisfied whenever\n"
            "  PREDECESSOR's state is STATE_ACTIVE. This is default.\n"
            "\n"
            "* \"negative\" means that the dependency is satisfied whenever\n"
            "  PREDECESSOR's state is STATE_INACTIVE instead of STATE_ACTIVE.\n"
            "\n"
            "* \"toggle\" means that a change in the prececessor's state toggles \n"
            "  this state's value. For this to work as expected you will also \n"
            "  want one of ACTIVE or INACTIVE, to toggle on the respective edge.\n"
            "\n"
            "* \"hard\" means that the dependency _must_ be satisfied for SWITCH to\n"
            "  be activated, even manually.\n"
            "\n"
            "* \"sufficient\" means that this dependency is sufficient to activate\n"
            "  the descendant even if other dependencies are not satisfied (i.e.,\n"
            "  it becomes an OR rather than AND gate).\n"
            "\n"
            "* By default, SWITCH's state is reevaluated whenever PREDECESSOR enters\n"
            "  one of its stable states: ACTIVE, INACTIVE, FAILED.\n"
            "  If one or more comma-separated STATE names are supplied here, the\n"
            "  evaluation will instead take place after the specified PREDECESSOR\n"
            "  state transitions.\n"
            "\n"
            "* Alternatively, the word \"manual\" means that SWITCH will not be\n"
            "  updated directly as a result of this dependency. (It will still be\n"
            "  considered whenever such reevaluations are triggered by other events\n"
            "  in the future.)\n"
            "\n"
            "Return value indicates whether the switch was added (true) or "
            "already existed (false); see \"--status\" above.",
            std::bind(&Options::add_dependency, this));

        this->add_command(
            "remove_dependency",
            {"SWITCH", "PREDECESSOR"},
            "Remove dependency on PREDECESSOR from SWITCH. Returns value indicates "
            "whether switch was removed; see \"--status\" above.",
            std::bind(&Options::remove_dependency, this));

        this->add_command(
            "add_interceptor",
            {"SWITCH", "NAME", "OWNER", "STATE[,...]", "COMMAND ..."},
            "Add a new interceptor with the specified NAME and OWNER on SWITCH",
            std::bind(&Options::add_interceptor, this));

        this->add_command(
            "remove_interceptor",
            {"SWITCH", "NAME"},
            "Remove interceptor with the specified NAME from SWITCH",
            std::bind(&Options::remove_interceptor, this));

        this->add_command(
            "clear_attributes",
            {"SWITCH"},
            "Clear all existing attributes on SWITCH.",
            std::bind(&Options::clear_attributes, this));

        this->add_command(
            "set_attributes",
            {"SWITCH", "[KEY VALUE] ..."},
            "Set attributes on SWITCH corresponding to the supplied KEY/VALUE pairs. "
            "Variant data types are inferred; to force VALUE to be interpreted as a "
            "string, use \"double quotation\" marks. (These may need to be further "
            "escaped in the user's shell).",
            std::bind(&Options::set_attributes, this));

        this->add_command(
            "set_target",
            {"SWITCH", "POSITION", "[no_intercept]", "[no_cascade]", "[clear]", "[KEY VALUE] ..."},
            "Set SWITCH's target to the specified boolean POSITION. "
            "Use `no_intercept` to skip invocation of associated interceptors, "
            "and `no_cascade` to skip cascading the updated state to descendants. "
            "Use `clear` to clear current attributes, and provide additional "
            "key/value pairs to assign new ones; see `set_attributes` for details.",
            //        "Also update its attributes from the provided KEY/VALUE pairs, if any.",
            std::bind(&Options::set_target, this));

        this->add_command(
            "activate",
            {"SWITCH", "[no_intercept]", "[no_cascade]", "[clear]", "[KEY VALUE] ..."},
            "Same as: set_target SWITCH ON ...",
            std::bind(&Options::set_active, this));

        this->add_command(
            "deactivate",
            {"SWITCH", "[no_intercept]", "[no_cascade]", "[clear]", "[KEY VALUE] ..."},
            "Same as: set_target SWITCH OFF ...",
            std::bind(&Options::set_inactive, this));

        this->add_command(
            "set_auto",
            {"SWITCH", "[no_intercept]", "[no_cascade]", "[clear]", "[KEY VALUE] ..."},
            "Set SWITCH's target position based on its dependencies. "
            "See `set_target` for a description of additional arguments.",
            //        "Also update its attributes from the provided KEY/VALUE pairs, if any.",
            std::bind(&Options::set_auto, this));

        this->add_command(
            "set_error",
            {"SWITCH", "MESSAGE", "[no_intercept]", "[no_cascade]", "[clear]", "[KEY VALUE] ..."},
            "Assign an error with text MESSAGE to SWITCH. "
            "See `set_target` for a description of additional arguments.",
            std::bind(&Options::set_error, this));

        this->add_command(
            "monitor",
            {"[except]", "[spec|status]", "..."},
            "Stay alive and watch for any specification/status updates from "
            "the server.  If one or more switches are listed, monitor updates "
            "only from those switches.",
            std::bind(&Options::monitor, this));

        this->add_command(
            "watch",
            {"[except]", "[spec|status]", "..."},
            "Alias for \"monitor\".",
            std::bind(&Options::monitor, this));
    }

    void Options::list_switches()
    {
        FlagMap flags;
        bool &with_state = flags["states"];
        this->get_flags(&flags, true);

        std::unordered_set<std::string> fields(
            this->current_arg,
            this->args.end());

        bool with_details = this->verbose | !fields.empty();
        const std::string state_field = "current_state";

        if (!fields.empty() && with_state)
        {
            fields.insert(state_field);
        }

        bool print_delimiter = false;
        for (const auto &[name, sw] : this->provider->get_switches())
        {
            if (with_details)
            {
                std::cout << "["
                          << name
                          << "]"
                          << std::endl;

                core::types::TaggedValueList details;
                sw->spec()->to_tvlist(&details);
                sw->status()->to_tvlist(&details);
                this->print_tvlist(details, fields);
                std::cout << std::endl;
            }
            else if (with_state)
            {
                std::cout << std::left
                          << std::setw(32)
                          << name
                          << ": "
                          << sw->state()
                          << std::endl;
            }
            else
            {
                std::cout << name
                          << std::endl;
            }
        }
    }

    void Options::get_exists()
    {
        this->report_status_and_exit(bool(this->get_switch(false)));
    }

    void Options::get_active()
    {
        this->report_status_and_exit(this->get_switch(true)->active());
    }

    void Options::get_inactive()
    {
        this->report_status_and_exit(!this->get_switch(true)->active());
    }

    void Options::get_culprits()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        FlagMap flags;
        bool &active_flag = flags["active"];
        bool &inactive_flag = flags["inactive"];
        this->get_flags(&flags, false);

        bool active = active_flag || !inactive_flag;

        for (const auto &[switch_name, state] : sw->culprits(active))
        {
            if (!this->verbose)
            {
                std::cout << switch_name << std::endl;
            }
            else if (auto culprit = this->provider->get_switch(switch_name))
            {
                std::cout << "["
                          << culprit->name()
                          << "]"
                          << std::endl;

                this->print_tvlist(culprit->status()->as_tvlist());

                std::cout << std::endl;
            }
        }
    }

    void Options::get_error()
    {
        if (core::status::Error::ptr error = this->get_switch(true)->error())
        {
            std::cout << *error << std::endl;
        }
    }

    void Options::get_errors()
    {
        for (const auto &[switch_name, error] : this->get_switch(true)->errors())
        {
            core::str::format(std::cout, "%20s : %s\n", switch_name, *error);
        }
    }

    void Options::get_status()
    {
        for (const auto &sw : this->get_switches_or_all())
        {
            std::cout << "["
                      << sw->name()
                      << "]"
                      << std::endl;

            this->print_tvlist(sw->status()->as_tvlist());

            std::cout << std::endl;
        }
    }

    void Options::get_localization()
    {
        std::cout << this->get_switch(true)->localization() << std::endl;
    }

    void Options::get_activate_text()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        std::cout << sw->activate_text() << std::endl;
    }

    void Options::get_deactivate_text()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        std::cout << sw->deactivate_text() << std::endl;
    }

    void Options::get_state_text()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        auto state = core::str::convert_to<switchboard::State>(this->get_arg("state"));
        std::cout << sw->state_text(state) << std::endl;
    }

    void Options::get_dependencies()
    {
        for (const auto &sw : this->get_switches_or_all())
        {
            std::cout << "["
                      << sw->name()
                      << "]"
                      << std::endl;

            for (const auto &[pred, dep] : sw->dependencies())
            {
                if (this->verbose)
                {
                    std::cout << " - " << *dep << std::endl;
                }
                else
                {
                    std::cout << " - " << pred << std::endl;
                }
            }

            std::cout << std::endl;
        }
    }

    void Options::get_interceptors()
    {
        for (const auto &sw : this->get_switches_or_all())
        {
            std::cout << "["
                      << sw->name()
                      << "]"
                      << std::endl;

            for (const auto &[name, icept] : sw->interceptors())
            {
                if (this->verbose)
                {
                    std::cout << " - " << icept << std::endl;
                }
                else
                {
                    std::cout << " - " << name << std::endl;
                }
            }

            std::cout << std::endl;
        }
    }

    void Options::get_specs()
    {
        for (const auto &sw : this->get_switches_or_all())
        {
            std::cout << "["
                      << sw->name()
                      << "]"
                      << std::endl;

            this->print_tvlist(sw->spec()->as_tvlist());

            std::cout << std::endl;
        }
    }

    void Options::load_file()
    {
        std::string filename = this->get_arg("FILENAME");
        this->provider->load(filename);
    }

    void Options::save_file()
    {
        std::string filename = this->get_arg("FILENAME");
        this->provider->save(filename);
    }

    void Options::add_switch()
    {
        std::string name = this->get_arg("SWITCH");
        FlagMap flags;
        bool &active = flags["active"];
        this->get_flags(&flags, false);

        auto [sw, added] = this->provider->add_switch(name, active);
        std::cout << (added ? "added" : "no change") << std::endl;
    }

    void Options::remove_switch()
    {
        std::string name = this->get_arg("SWITCH");
        bool removed = this->provider->remove_switch(name);
        std::cout << (removed ? "removed" : "no change") << std::endl;
    }

    void Options::clear_switches()
    {
        FlagMap flags;
        bool &reload = flags["reload"];
        this->get_flags(&flags, false);

        bool cleared = this->provider->clear_switches(reload);
        std::cout << (cleared ? "cleared" : "no change") << std::endl;
    }

    void Options::add_alias()
    {
        switchboard::SwitchRef sw = this->get_switch(true);

        switchboard::SwitchAliases aliases = {this->get_arg("ALIAS ...")};
        while (auto arg = this->next_arg())
        {
            aliases.insert(*arg);
        }

        sw->add_aliases(aliases);
    }

    void Options::remove_alias()
    {
        switchboard::SwitchRef sw = this->get_switch(true);

        switchboard::SwitchAliases aliases = {this->get_arg("ALIAS ...")};
        while (auto arg = this->next_arg())
        {
            aliases.insert(*arg);
        }

        sw->remove_aliases(aliases);
    }

    void Options::set_localization()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        std::string language = this->get_arg("language");
        switchboard::Localization localization;

        for (const auto &[key, value] : this->get_attributes(false))
        {
            std::string lowerkey = core::str::tolower(key);

            if (lowerkey == "text")
                localization.description = value;
            else if (lowerkey == "on")
                localization.activate_text = value;
            else if (lowerkey == "off")
                localization.deactivate_text = value;
            else if (lowerkey == "activating")
                localization.state_texts[switchboard::STATE_ACTIVATING] = value;
            else if (lowerkey == "active")
                localization.state_texts[switchboard::STATE_ACTIVE] = value;
            else if (lowerkey == "deactivating")
                localization.state_texts[switchboard::STATE_DEACTIVATING] = value;
            else if (lowerkey == "inactive")
                localization.state_texts[switchboard::STATE_INACTIVE] = value;
            else if (lowerkey == "failing")
                localization.state_texts[switchboard::STATE_FAILING] = value;
            else if (lowerkey == "failed")
                localization.state_texts[switchboard::STATE_FAILED] = value;
            else
                throwf_args(core::exception::InvalidArgument,
                            ("Unknown specification field: %r", key),
                            key);
        }
        //    Switchboard::LocalizationMap({{language, localization}};
        sw->set_localizations({{language, localization}});
    }

    void Options::add_dependency()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        std::string pred_name = this->get_arg("predecessor state");

        std::vector<std::string> remaining;
        FlagMap flags;
        bool &positive = flags["positive"];
        bool &negative = flags["negative"];
        bool &toggle = flags["toggle"];
        bool &hard = flags["hard"];
        bool &sufficient = flags["sufficient"];
        bool &manual = flags["manual"];
        this->get_flags(&flags, true);

        switchboard::StateSet trigger_states =
            this->args.size() ? this->get_states()
            : manual          ? switchboard::StateSet()
                              : switchboard::Dependency::DEFAULT_TRIGGERS;

        switchboard::DependencyPolarity dep_dir =
            positive   ? switchboard::DependencyPolarity::POSITIVE
            : negative ? switchboard::DependencyPolarity::NEGATIVE
            : toggle   ? switchboard::DependencyPolarity::TOGGLE
                       : switchboard::DependencyPolarity::POSITIVE;

        auto dep = switchboard::Dependency::create_shared(
            this->provider,
            pred_name,
            trigger_states,
            dep_dir,
            hard,
            sufficient);
        bool status = sw->add_dependency(dep);
        this->report_status_and_exit(status);
    }

    void Options::remove_dependency()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        std::string pred_name = this->get_arg("predecessor");
        bool status = sw->remove_dependency(pred_name);
        this->report_status_and_exit(status);
    }

    void Options::add_interceptor()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        std::string name = this->get_arg("name");
        std::string owner = this->get_arg("owner");
        switchboard::StateSet state_transitions = this->get_states();

        auto icept = switchboard::Interceptor::create_shared(
            name,
            owner,
            [=](switchboard::SwitchRef s, switchboard::State state) {
                logf_info("State %s interceptor %s triggered on %s transition",
                          s->name(),
                          name,
                          state);
            },
            state_transitions);

        bool status = sw->add_interceptor(icept);
        this->report_status_and_exit(status);
    }

    void Options::remove_interceptor()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        std::string name = this->get_arg("interceptor name");
        bool status = sw->remove_interceptor(name);
        this->report_status_and_exit(status);
    }

    void Options::clear_attributes()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        sw->set_attributes({}, true);
    }

    void Options::set_attributes()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        core::types::KeyValueMap attributes = this->get_attributes(true);
        sw->set_attributes(attributes, false);
    }

    void Options::set_target()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        bool target = core::str::convert_to<bool>(this->get_arg("boolean target position"));
        SwitchControlFlags flags = this->get_switch_control_flags();
        sw->set_active(target,
                       this->get_attributes(false),
                       flags.clear_existing,
                       flags.invoke_interceptors,
                       flags.cascade_style,
                       flags.reenter);
    }

    void Options::set_active()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        SwitchControlFlags flags = this->get_switch_control_flags();
        sw->set_active(true,
                       this->get_attributes(false),
                       flags.clear_existing,
                       flags.invoke_interceptors,
                       flags.cascade_style,
                       flags.reenter);
    }

    void Options::set_inactive()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        SwitchControlFlags flags = this->get_switch_control_flags();
        sw->set_active(false,
                       this->get_attributes(false),
                       flags.clear_existing,
                       flags.invoke_interceptors,
                       flags.cascade_style,
                       flags.reenter);
    }

    void Options::set_auto()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        SwitchControlFlags flags = this->get_switch_control_flags();
        sw->set_auto(this->get_attributes(false),
                     flags.clear_existing,
                     flags.invoke_interceptors,
                     flags.cascade_style,
                     flags.reenter);
    }

    void Options::set_error()
    {
        switchboard::SwitchRef sw = this->get_switch(true);
        core::exception::RuntimeError error(this->get_arg("error message"));

        SwitchControlFlags flags = this->get_switch_control_flags();
        sw->set_error(core::exception::map_to_error(error),
                      this->get_attributes(false),
                      flags.clear_existing,
                      flags.invoke_interceptors,
                      flags.cascade_style,
                      flags.reenter);
    }

    void Options::on_monitor_start()
    {
        FlagMap flags;
        bool &except = flags["except"];
        bool &show_spec = flags["spec"];
        bool &show_status = flags["status"];
        this->get_flags(&flags, false);

        except |= std::none_of(
            flags.begin(),
            flags.end(),
            [](auto kv) {
                return kv.second;
            });

        if (show_spec != except)
        {
            switchboard::signal_spec.connect(this->signal_handle, on_signal_spec);
        }

        if (show_status != except)
        {
            switchboard::signal_status.connect(this->signal_handle, on_signal_status);
        }

        this->provider->wait_ready();
    }

    void Options::on_monitor_end()
    {
        switchboard::signal_spec.disconnect(this->signal_handle);
        switchboard::signal_status.disconnect(this->signal_handle);
    }

    void Options::on_signal_spec(core::signal::MappingAction action,
                                 const switchboard::SwitchName &name,
                                 const switchboard::Specification &spec)
    {
        core::str::format(std::cout,
                          "[%s] spec:   %-12s %-32r %s\n",
                          core::dt::to_string(core::dt::Clock::now(), 3, "%T"),
                          action,
                          name,
                          spec);
    }

    void Options::on_signal_status(core::signal::MappingAction action,
                                   const switchboard::SwitchName &name,
                                   const switchboard::Status &status)
    {
        core::str::format(std::cout,
                          "[%s] status: %-12s %-32r %s\n",
                          core::dt::to_string(core::dt::Clock::now(), 3, "%T"),
                          action,
                          name,
                          status);
    }

    std::vector<switchboard::SwitchRef> Options::get_switches_or_all()
    {
        std::vector<std::string> patterns(this->current_arg, this->args.end());

        if (patterns.empty())
        {
            return this->provider->get_switches().values();
        }
        else
        {
            switchboard::SwitchSelection selection(patterns);
            std::vector<switchboard::SwitchRef> switches;

            for (const auto &[name, sw] : this->provider->get_switches())
            {
                if (selection.matches(name))
                {
                    switches.push_back(sw);
                }
            }
            return switches;
        }
    }

    switchboard::SwitchRef Options::get_switch(bool required)
    {
        std::optional<std::string> switch_name;
        if (required)
        {
            switch_name = this->get_arg("SWITCH");
        }
        else
        {
            switch_name = this->next_arg();
        }

        if (switch_name)
        {
            return this->provider->get_switch(switch_name.value(), true);
        }
        else
        {
            return {};
        }
    }

    Options::SwitchControlFlags Options::get_switch_control_flags()
    {
        FlagMap flags;
        bool &no_intercept = flags["no_intercept"];
        bool &no_cascade = flags["no_cascaade"];
        bool &clear = flags["clear"];
        bool &reenter = flags["reenter"];
        this->get_flags(&flags, true);

        return {
            .clear_existing = clear,
            .invoke_interceptors = no_intercept
                                     ? switchboard::InvocationStyle::INDIRECT
                                     : switchboard::InvocationStyle::ALL,
            .cascade_style = no_cascade
                               ? switchboard::CascadeStyle::NONE
                               : switchboard::CascadeStyle::DEFAULT,
            .reenter = reenter,
        };
    }

    switchboard::StateSet Options::get_states()
    {
        std::string arg = this->get_arg("LIST_OF_STATES");
        switchboard::StateSet states;
        for (const std::string &state_name : core::str::split(arg, ","))
        {
            states.insert(core::str::convert_to<switchboard::State>(state_name));
        }
        return states;
    }

    void Options::print_states() const
    {
        for (const auto &[state, name] : switchboard::state_names)
        {
            std::cout << name << std::endl;
        }
    }

    void Options::print_tvlist(
        const core::types::TaggedValueList &tvlist,
        const std::unordered_set<std::string> &selection,
        std::size_t alignment_column) const
    {
        for (const auto &[tag, value] : tvlist)
        {
            std::string key = tag.value_or("");
            if (selection.empty() || selection.count(key))
            {
                std::cout << std::right
                          << std::setw(alignment_column)
                          << key
                          << std::left
                          << ": "
                          << value
                          << std::endl;
            }
        }
    }
}  // namespace cc::platform::switchboard
