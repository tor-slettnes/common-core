// -*- c++ -*-
//==============================================================================
/// @file switchboard-provider.h++
/// @brief Abstract switchboard provider API
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switch.h++"
#include "thread/signaltemplate.h++"

#include <memory>

namespace switchboard
{
    //==========================================================================
    // Provider

    class Provider : public std::enable_shared_from_this<Provider>
    {
        using This = Provider;

    protected:
        Provider(const std::string &implementation);

    public:  // Operations to manage switches
        const std::string &implementation() const;

        virtual void initialize() {}
        virtual void deinitialize() {}
        virtual bool available() const = 0;
        virtual bool wait_ready() const = 0;

        bool load(const fs::path &filename,
                  bool replace_specifications = false,
                  bool replace_statuses = true);

        bool save(const fs::path &filename,
                  bool include_specifications = false,
                  bool include_statuses = true);

        /// @returns
        ///    Iterator to beginning of Switch map.
        virtual SwitchMap::const_iterator begin() const;
        virtual SwitchMap::iterator begin();

        ///    Iterator to end of Switch map.
        virtual SwitchMap::const_iterator end() const;
        virtual SwitchMap::iterator end();

        /// @brief
        ///    Get a map of all switches
        /// @return
        ///    A shallow copy of available switches
        virtual SwitchMap get_switches() const;

        /// @brief
        ///    Find an existing switch, or create a new one if missing
        /// @param[in] name
        ///    Switch name
        /// @param[in] active
        ///    Initial position, if added
        virtual SwitchRef get_or_add_switch(
            const SwitchName &name,
            bool active = false);

        /// @brief Find a switch
        /// @param[in] name
        ///    Switch name
        /// @returns
        ///    Iterator to the switch if found, or this->end() otherwise.
        virtual SwitchMap::const_iterator find(
            const SwitchName &name) const;

        virtual SwitchMap::iterator find(
            const SwitchName &name);

        /// @brief Find a switch
        /// @param[in] name
        ///    Switch name
        /// @param[in] required
        ///    Determines behavior if switch is not found: If `true` throw an
        ///    `std::out_of_range` exception, otherwise return an empty
        ///    reference.
        virtual SwitchRef get_switch(
            const SwitchName &name,
            bool required = false) const;

        /// @brief Add a new switch, or update an existing one
        /// @param[in] name
        ///    Switch name
        /// @param[in] active
        ///    Initial position
        virtual std::pair<SwitchRef, bool> add_switch(
            const SwitchName &name,
            bool active = false) = 0;

        /// @brief Remove an existing switch
        /// @param[in] name
        ///    Switch name
        /// @param[in] propagate
        ///    Reevaluate descendants after removal
        /// @return
        ///    Boolean indicating whether switch existed or not
        virtual bool remove_switch(
            const SwitchName &name,
            bool propagate = true) = 0;

        /// @brief Remove all switches, optionally reloading defaults
        /// @param[in] reload
        ///    Reload default switches after clearing.
        /// @returns
        ///    Boolean indicating whether any changes were made
        virtual bool clear_switches(
            bool reload = false) = 0;

        /// @brief Import switches from a key/value map
        /// @param[in] declarations
        ///     Switch names mapped to corresponding declarations, structured
        ///     like those those in a declarations file.
        /// @param[in] include_specifications
        ///     Parse and update switch specifications from the provided maps.
        /// @param[in] include_specs
        ///     Parse and update switch statuses from the provided maps.

        virtual uint import_switches(
            const core::types::KeyValueMap &declarations,
            bool replace_specifications = false,
            bool replace_statuses = true) = 0;

        /// @brief Export switches to a key/value map
        /// @param[in] selection
        ///     Switch name patterns to include in export.
        ///     If not provided, all switches are exported.
        /// @param[in] include_specifications
        ///     Include specifications in the generated map
        /// @param[in] include_status
        ///     Include current switch statuses in generated map
        /// @return
        ///     Switch names mapped to corresponding declarations.
        ///     These may subsequently be imported.

        virtual core::types::KeyValueMap export_switches(
            const std::optional<SwitchSelection> &selection,
            bool include_specifications = false,
            bool include_statuses = true) const = 0;

        /// @brief
        ///      Add an interceptor with a callback handler to be invoked
        ///      following an applicable state change.
        /// @param[in] interceptor
        ///      Interceptor to be invoked after each matching state change.
        /// @param[in] switch_seleciton
        ///      Switches to which this intereceptor will be added.
        /// @param[in] immediate
        ///      If the interceptor's trigger states include this switch's
        ///      current state OR the transitional state preceding it (for
        ///      instance, if the switch is currently ACTIVE and the interceptor
        ///      triggers on either ACTIVATING and ACTIVE), invoke it
        ///      immediately.  In this case, unless the interceptor's
        ///      `asynchronous` flag is also True, the call blocks until the
        ///      interceptor has completed.
        /// @return
        ///     `true` if the interceptor was added, `false` if the name already existed.

        virtual bool add_interceptor(
            const InterceptorRef &interceptor,
            const SwitchSelection &switch_selection,
            bool immediate = false,
            bool future = true) = 0;

        /// @brief
        ///     Remove an existing interceptor
        /// @param[in] name
        ///     Interceptor name.
        /// @param[in] switch_selection
        ///     Specific switches from which this interceptor will be removed.
        ///     By default it is removed from all existing switches, and will
        ///     not be added to switches created in the future.
        /// @returns
        ///     `true` if a removal took place.

        virtual bool remove_interceptor(
            const InterceptorName &name,
            const std::optional<SwitchSelection> &switch_selection = {}) = 0;

    protected:
        /// @brief
        ///    Add or remove a switch based on a mapping signal
        /// @param[in] action
        ///    Whether to add/update or remove switch
        /// @param[in] switch_name
        ///    Switch name

        template <class SwitchType>
        SwitchRef sync_switch(core::signal::MappingAction action,
                              const SwitchName &switch_name)
        {
            switch (action)
            {
            case core::signal::MAP_ADDITION:
            case core::signal::MAP_UPDATE:
            {
                auto [ref, inserted] = this->find_or_insert<SwitchType>(
                    switch_name,
                    this->shared_from_this());
                logf_trace("Switch %r, inserted = %b", *ref, inserted);
                return ref;
            }

            case core::signal::MAP_REMOVAL:
                this->switches.erase(switch_name);
                return {};

            default:
                return {};
            }
        }

        /// @brief Find a switch of the specified type, or add one if missing.
        /// @param[in] switch_name
        ///    Switch name
        /// @param[in] args
        ///    Additional arguments, passed onto the `Switch()` constructor
        ///    if a new switch is created.

        template <class SwitchType, class... Args>
        std::pair<std::shared_ptr<SwitchType>, bool>
        find_or_insert(const std::string &switch_name, Args &&...args)
        {
            if (auto sw = std::dynamic_pointer_cast<SwitchType>(this->switches.get(switch_name)))
            {
                return {sw, false};
            }
            else
            {
                sw = SwitchType::create_shared(switch_name, std::forward<Args>(args)...);
                this->switches.insert_or_assign(switch_name, sw);
                return {sw, true};
            }
        }

    private:
        std::string implementation_;

    protected:
        SwitchMap switches;
        std::recursive_mutex switches_mutex;
    };

    extern std::shared_ptr<Provider> provider;

}  // namespace switchboard
