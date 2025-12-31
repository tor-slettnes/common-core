// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-host.h++
/// @brief SysConfig native implementation - Host Configuration
/// @author Tor Slettnes
//==============================================================================

#include "sysconfig-host.h++"

namespace sysconfig::native
{
    class HostConfigProvider : public HostConfigInterface
    {
        using This = HostConfigProvider;
        using Super = HostConfigInterface;

    public:
        HostConfigProvider();

        void initialize() override;

        HostInfo get_host_info() const override;
        void set_host_name(const std::string &hostname) override;
        void reboot() override;

    private:
        void emit() const;
    };
}  // namespace sysconfig::native
