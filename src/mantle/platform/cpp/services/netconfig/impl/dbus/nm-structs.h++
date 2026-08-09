/// -*- c++ -*-
//==============================================================================
/// @file nm-structs.h++
/// @brief Network Manager object wrapper
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "netconfig-types.h++"
#include "glib-variant.h++"

#include <unordered_map>

namespace cc::platform::netconfig::dbus
{
    void extract_addressdata(
        const cc::glib::VariantMaps& configs,
        AddressVector* addresses);

    namespace connection
    {
        void extract_settings(const cc::glib::SettingsContainer& settings,
                              ConnectionData* data);

        void insert_settings(const ConnectionData& data,
                             cc::glib::SettingsMap* settings);

        cc::glib::SettingsContainer build_settings_container(
            const ConnectionData& data);
    }  // namespace connection
}  // namespace cc::platform::netconfig::dbus
