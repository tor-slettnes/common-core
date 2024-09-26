/// -*- c++ -*-
//==============================================================================
/// @file linux-dns-sd-avahi.c++
/// @brief DNS service discovery though Avahi daemon
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "linux-dns-sd-avahi.h++"
#include "platform/host.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

namespace core::platform
{
    AvahiServiceDiscoveryProvider::AvahiServiceDiscoveryProvider()
        : Super("AvahiServiceDiscoveryProvider"),
          group(nullptr),
          simple_poll(avahi_simple_poll_new()),
          client_error(0),
          client(this->simple_poll
                     ? avahi_client_new(
                           avahi_simple_poll_get(this->simple_poll),
                           static_cast<AvahiClientFlags>(0),
                           AvahiServiceDiscoveryProvider::client_callback,
                           this,
                           &this->client_error)
                     : nullptr)
    {
    }

    AvahiServiceDiscoveryProvider::~AvahiServiceDiscoveryProvider()
    {
        if (this->client)
        {
            avahi_client_free(this->client);
        }

        if (this->simple_poll)
        {
            avahi_simple_poll_free(this->simple_poll);
        }
    }

    bool AvahiServiceDiscoveryProvider::is_pertinent() const
    {
        return this->client != nullptr;
    }

    void AvahiServiceDiscoveryProvider::initialize()
    {
        Super::initialize();
    }

    void AvahiServiceDiscoveryProvider::deinitialize()
    {
        this->reset();
        Super::deinitialize();
    }

    void AvahiServiceDiscoveryProvider::advertise_service(
        const std::string &name,
        const std::string &type,
        uint port,
        const AttributeMap &attributes)
    {
        if (!this->client)
        {
            throwf(core::exception::FailedPrecondition,
                   "Could not create Avahi client: %s",
                   avahi_strerror(this->client_error));
        }

        this->add_service({
            .name = name,
            .type = type,
            .port = port,
            .attributes = attributes,
        });
    }

    void AvahiServiceDiscoveryProvider::commit()
    {
        if (this->group)
        {
            int status = avahi_entry_group_commit(this->group);
            if (status < 0)
            {
                throwf(core::exception::RuntimeError,
                       "AvahiClient failed to commit entry group: %s",
                       avahi_strerror(status));
            }
        }
    }

    void AvahiServiceDiscoveryProvider::reset()
    {
        if (this->group)
        {
            avahi_entry_group_reset(this->group);
        }
    }


    void AvahiServiceDiscoveryProvider::create_entry_group()
    {
        if (!this->group)
        {
            this->group = avahi_entry_group_new(this->client,
                                                This::entry_group_callback,
                                                this);
        }

        if (!this->group)
        {
            avahi_simple_poll_quit(this->simple_poll);
            throwf(core::exception::FailedPrecondition,
                   "Could not create Avahi entry group: %s",
                   avahi_strerror(avahi_client_errno(client)));
        }
    }

    void AvahiServiceDiscoveryProvider::add_service(
        const ServiceInfo &service_info)
    {
        this->create_entry_group();

        AvahiStringList *strings = This::avahi_string_list(service_info.attributes);

        int status = avahi_entry_group_add_service_strlst(
            this->group,                        // group
            AVAHI_IF_UNSPEC,                    // interface
            AVAHI_PROTO_UNSPEC,                 // protocol
            static_cast<AvahiPublishFlags>(0),  // flags
            service_info.name.c_str(),          // name
            service_info.type.c_str(),          // type
            nullptr,                            // domain
            nullptr,                            // host
            service_info.port,                  // port
            strings);                           // txt

        avahi_string_list_free(strings);

        if (status < 0)
        {
            throwf(core::exception::RuntimeError,
                   "AvahiClient failed to add service %s: %s",
                   service_info.name,
                   avahi_strerror(status));
        }
    }

    void AvahiServiceDiscoveryProvider::client_callback(
        AvahiClient *client,
        AvahiClientState state,
        void *userdata)
    {
        auto instance = reinterpret_cast<AvahiServiceDiscoveryProvider *>(userdata);

        switch (state)
        {
        case AVAHI_CLIENT_S_RUNNING:
            /* The server has startup successfully and registered its host
             * name on the network, so it's time to create our services */
            // create_services(c);
            break;

        case AVAHI_CLIENT_FAILURE:
            avahi_simple_poll_quit(instance->simple_poll);

            break;

        case AVAHI_CLIENT_S_COLLISION:
            /* Let's drop our registered services. When the server is back
             * in AVAHI_SERVER_RUNNING state we will register them
             * again with the new host name. */

        case AVAHI_CLIENT_S_REGISTERING:
            /* The server records are now being established. This
             * might be caused by a host name change. We need to wait
             * for our own records to register until the host name is
             * properly esatblished. */

            if (instance->group)
            {
                avahi_entry_group_reset(instance->group);
            }

            break;

        case AVAHI_CLIENT_CONNECTING:
            break;
        }
    }

    void AvahiServiceDiscoveryProvider::entry_group_callback(
        AvahiEntryGroup *group,
        AvahiEntryGroupState state,
        void *userdata)
    {
        auto instance = reinterpret_cast<AvahiServiceDiscoveryProvider *>(userdata);

        switch (state)
        {
        case AVAHI_ENTRY_GROUP_ESTABLISHED:
            break;

        case AVAHI_ENTRY_GROUP_COLLISION:
        {
            /* And recreate the services */
            // This::create_services(avahi_entry_group_get_client(group));
            break;
        }

        case AVAHI_ENTRY_GROUP_FAILURE:
            /* Some kind of failure happened while we were registering our services */
            avahi_simple_poll_quit(instance->simple_poll);
            break;

        case AVAHI_ENTRY_GROUP_UNCOMMITED:
            break;

        case AVAHI_ENTRY_GROUP_REGISTERING:;
            break;
        }
    }

    AvahiStringList *AvahiServiceDiscoveryProvider::avahi_string_list(
        const AttributeMap &attributes)
    {
        std::vector<std::string> strings;
        strings.reserve(attributes.size());

        for (const auto &[key, value] : attributes)
        {
            std::string &s = strings.emplace_back(key + "=" + value);
        }

        // We use an old-fashioned C array instead of `std::vector<>`,
        // because if there are no elements, `vector<>::data()` returns `nullptr`.
        const char *c_strings[attributes.size()];
        for (uint c = 0; c < strings.size(); c++)
        {
            c_strings[c] = strings.at(c).c_str();
        }

        return avahi_string_list_new_from_array(c_strings, strings.size());
    }

}  // namespace core::platform
