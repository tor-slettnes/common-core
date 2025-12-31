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

namespace netconfig::dbus
{
    void extract_addressdata(
        const core::glib::VariantMaps &configs,
        AddressVector *addresses);

    namespace connection
    {
        void extract_settings(const core::glib::SettingsContainer &settings,
                              ConnectionData *data);

        void insert_settings(const ConnectionData &data,
                             core::glib::SettingsMap *settings);

        core::glib::SettingsContainer build_settings_container(
            const ConnectionData &data);
    }  // namespace connection
}  // namespace netconfig::dbus
