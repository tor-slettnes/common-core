// -*- c++ -*-
//==============================================================================
/// @file system-native-host.h++
/// @brief System service - Host Configuration Native Implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-api-host.h++"

namespace platform::system::native
{
    class HostConfig : public HostConfigProvider
    {
        using Super = HostConfigProvider;

    public:
        HostConfig();

        HostInfo get_host_info() const override;
        void set_host_name(const std::string &hostname) override;
        void reboot() override;
    };
}  // namespace platform::system::native
