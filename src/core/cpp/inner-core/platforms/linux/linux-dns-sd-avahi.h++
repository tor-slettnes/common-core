/// -*- c++ -*-
//==============================================================================
/// @file linux-dns-sd-avahi.h++
/// @brief DNS service discovery though Avahi daemon
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "platform/dns-sd.h++"
#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/alternative.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>

namespace core::platform
{
    /// @brief Abstract provider for service discovery related functions
    class AvahiServiceDiscoveryProvider : public ServiceDiscoveryProvider
    {
        using This = AvahiServiceDiscoveryProvider;
        using Super = ServiceDiscoveryProvider;

    protected:
        struct ServiceInfo
        {
            std::string name;
            std::string type;
            uint port;
            AttributeMap attributes;
        };

    protected:
        AvahiServiceDiscoveryProvider();
        ~AvahiServiceDiscoveryProvider();

    public:
        bool is_pertinent() const override;
        void initialize() override;
        void deinitialize() override;

        void add_service(
            const std::string &name,
            const std::string &type,
            uint port,
            const AttributeMap &attributes) override;

        void add_service_subtype(
            const std::string &name,
            const std::string &type,
            const std::string &subtype) override;

        void commit() override;

        void reset() override;

    private:
        void create_entry_group();

        void add_service_info(const ServiceInfo &service_info);

        static AvahiStringList *avahi_string_list(
            const AttributeMap &attributes);

        static void client_callback(
            AvahiClient *client,
            AvahiClientState state,
            void *userdata);

        static void entry_group_callback(
            AvahiEntryGroup *group,
            AvahiEntryGroupState state,
            void *userdata);

    private:
        AvahiEntryGroup *group;
        AvahiSimplePoll *simple_poll;
        int client_error;
        AvahiClient *client;
    };
}  // namespace core::platform
