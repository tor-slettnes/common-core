// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-host.h++
/// @brief SysConfig service - Host Configuration Native Implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-host.h++"

namespace platform::sysconfig::native
{
    class HostConfigProvider : public HostConfigInterface
    {
        using This = HostConfigProvider;
        using Super = HostConfigInterface;

    public:
        HostConfigProvider();

        HostInfo get_host_info() const override;
        void set_host_name(const std::string &hostname) override;
        void reboot() override;
    };
}  // namespace platform::sysconfig::native
