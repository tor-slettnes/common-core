// -*- c++ -*-
//==============================================================================
/// @file remote-grpc-switch.h++
/// @brief Switch controlled via a remote gRPC service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "switchboard-grpc-proxy.h++"
#include "switch.h++"
#include "types/create-shared.h++"

namespace switchboard::grpc
{
    //==========================================================================
    /// @class RemoteSwitch
    /// @brief Switch managed remotely by Switchboard Service via gRPC
    ///
    /// @note
    ///     Specification and Status are cached locally, and updated from
    ///     server signals.

    class RemoteSwitch : public Switch,
                         public core::types::enable_create_shared<RemoteSwitch>
    {
        using Client = core::grpc::ClientWrapper<cc::platform::switchboard::grpc::Switchboard>;
        // friend class Proxy;

    protected:
        using Switch::Switch;

    protected:
        std::shared_ptr<Client> client() const;

    protected:
        bool add_dependency(
            const DependencyRef &dependency,
            bool allow_update,
            bool reevaluate) override;

        bool remove_dependency(
            SwitchName predecessor_name,
            bool reevaluate = true) override;

        bool add_interceptor(
            const InterceptorRef &interceptor,
            bool immediate = false) override;
        bool remove_interceptor(
            const InterceptorName &id) override;

        void update_spec(
            const std::optional<bool> &primary,
            const LocalizationMap &localizations,
            bool replace_localizations,
            const DependencyMap &dependencies,
            bool replace_dependencies,
            const InterceptorMap &interceptors,
            bool replace_interceptors,
            bool update_state) override;

        bool set_target(
            State target_state,
            const core::status::Error::ptr &error = {},
            const core::types::KeyValueMap &attributes = {},
            bool clear_existing = false,
            bool with_interceptors = true,
            bool trigger_descendants = true,
            bool reevaluate = false,
            ExceptionHandling on_cancel = ExceptionHandling::EH_DEFAULT,
            ExceptionHandling on_error = ExceptionHandling::EH_DEFAULT) override;

        bool set_attributes(
            const core::types::KeyValueMap &attributes,
            bool clear_existing) override;
    };

}  // namespace switchboard
