// -*- c++ -*-
//==============================================================================
/// @file switchboard-provider.h++
/// @brief Abstract switchboard provider API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "switch.h++"

#include <memory>

namespace switchboard
{
    //==========================================================================
    // Provider

    class Provider : public std::enable_shared_from_this<Provider>
    {
    protected:
        Provider(const std::string &implementation);

    public:  // Operations to manage switches
        const std::string &implementation() const;

        virtual void initialize() {}
        virtual void deinitialize() {}
        virtual bool available() const = 0;
        virtual bool wait_ready() const = 0;

        bool load(const fs::path &filename);

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
        virtual SwitchRef get_or_add_switch(const SwitchName &name);

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
        virtual std::pair<SwitchRef, bool> add_switch(const SwitchName &name) = 0;

        /// @brief Remove an existing switch
        /// @param[in] name
        ///    Switch name
        /// @param[in] propagate
        ///    Reevaluate descendents after removal
        /// @return
        ///    Boolean indicating whether switch existed or not
        virtual bool remove_switch(
            const SwitchName &name,
            bool propagate = true) = 0;

    public:
        // Operations pertaining to indivdiual switches

        /// @brief
        ///     Add an interceptor that takes the switch reference and
        ///     newly acquired state as arguments
        template <class... Args>
        std::pair<InterceptorRef, bool> inline emplace_interceptor(
            const SwitchName &switch_name,
            const InterceptorName &interceptor_name,
            const std::function<void(SwitchRef sw, State target_state)> &invocation,
            bool immediate,
            Args &&...args)
        {
            auto [sw, inserted] = this->add_switch(switch_name);
            if (InterceptorRef ref = sw->get_interceptor(interceptor_name))
            {
                return {ref, false};
            }
            else
            {
                ref = Interceptor::create_shared(
                    interceptor_name,
                    "",
                    invocation,
                    std::forward<Args>(args)...);
                sw->add_interceptor(ref, immediate);
                return {ref, true};
            }
        }

        /// @brief
        ///     Remove an existing interceptor
        bool remove_interceptor(const SwitchName &switch_name,
                                const InterceptorName &interceptor_name);

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
        static Specification import_spec(
            const SwitchRef &sw,
            const core::types::KeyValueMap::ptr &kvmap);

        static Description import_desc(
            const core::types::KeyValueMap::ptr &kvmap);

        static DependencyRef import_dependency(
            const SwitchRef &sw,
            const std::string &predecessor_name,
            const core::types::KeyValueMap::ptr &dep_map);

    private:
        std::string implementation_;

    protected:
        SwitchMap switches;
    };

}  // namespace switchboard
