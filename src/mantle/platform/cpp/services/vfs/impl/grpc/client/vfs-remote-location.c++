// -*- c++ -*-
//==============================================================================
/// @file vfs-remote-location.h++
/// @brief VFS service - remote filesytem location handle
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-remote-location.h++"
#include "vfs-remote-context.h++"
#include "protobuf-vfs-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

namespace platform::vfs
{
    //==========================================================================
    // RemoteLocation

    VolumeStats RemoteLocation::volume_stats(
        const OperationFlags &flags) const
    {
        protobuf::decoded<VolumeStats>(
            this->client->call_check(
                &Client::Stub::volume_stats,
                protobuf::encoded<cc::platform::vfs::PathRequest>(
                    this->virtualPath(),
                    flags)));
    }

    FileStats RemoteLocation::file_stats(
        const OperationFlags &flags) const
    {
        return protobuf::decoded<FileStats>(
            this->client->call_check(
                &Client::Stub::file_stats,
                protobuf::encoded<cc::platform::vfs::PathRequest>(
                    this->virtualPath(),
                    flags)));
    }

    Directory RemoteLocation::list_folder(
        const OperationFlags &flags) const
    {
        return protobuf::decoded<Directory>(
            this->client->call_check(
                &Client::Stub::get_directory,
                protobuf::encoded<cc::platform::vfs::PathRequest>(
                    this->virtualPath(),
                    flags)));
    }

    Directory RemoteLocation::locate(
        const std::vector<std::string> &filename_masks,
        const core::types::TaggedValueList &attribute_filters,
        const OperationFlags &flags) const
    {
        return protobuf::decoded<Directory>(
            this->client->call_check(
                &Client::Stub::locate,
                protobuf::encoded<cc::platform::vfs::LocateRequest>(
                    this->virtualPath(),
                    filename_masks,
                    attribute_filters,
                    flags)));
    }

    void RemoteLocation::copy_from(
        const LocationList &sources,
        const OperationFlags &flags) const
    {
        this->client->call_check(
            &Client::Stub::copy,
            protobuf::encoded<cc::platform::vfs::PathRequest>(
                this->virtualPath(),
                this->location_paths(sources),
                flags));
    }

    void RemoteLocation::move_from(
        const LocationList &sources,
        const OperationFlags &flags) const
    {
        this->client->call_check(
            &Client::Stub::move,
            protobuf::encoded<cc::platform::vfs::PathRequest>(
                this->virtualPath(),
                this->location_paths(sources),
                flags));
    }

    void RemoteLocation::remove(
        const OperationFlags &flags) const
    {
        this->client->call_check(
            &Client::Stub::remove,
            protobuf::encoded<cc::platform::vfs::PathRequest>(
                this->virtualPath(),
                flags));
    }

    void RemoteLocation::create_folder(
        const OperationFlags &flags) const
    {
        this->client->call_check(
            &Client::Stub::create_folder,
            protobuf::encoded<cc::platform::vfs::PathRequest>(
                this->virtualPath(),
                flags));
    }

    UniqueReader RemoteLocation::read() const
    {
        return std::make_unique<ClientInputStream>(
            this->client->stub,
            this->virtualPath());
    }

    UniqueWriter RemoteLocation::write() const
    {
        return std::make_unique<ClientOutputStream>(
            this->client->stub,
            this->virtualPath());
    }

    core::types::KeyValueMap RemoteLocation::get_attributes() const
    {
        return protobuf::decoded<core::types::KeyValueMap>(
            this->client->call_check(
                &Client::Stub::get_attributes,
                protobuf::encoded<cc::platform::vfs::Path>(
                    this->virtualPath())));
    }

    void RemoteLocation::set_attributes(
        const core::types::KeyValueMap &attributes) const
    {
        this->client->call_check(
            &Client::Stub::set_attributes,
            protobuf::encoded<cc::platform::vfs::AttributeRequest>(
                this->virtualPath(),
                attributes);
    }

    void RemoteLocation::clear_attributes() const
    {
        this->client->call_check(
            &Client::Stub::clear_attributes,
            protobuf::encoded<cc::platform::vfs::Path>(
                this->virtualPath()));
    }

    //==========================================================================
    // Private methods

    PathList RemoteLocations::location_paths(const LocationList &locations)
    {
        PathList paths;
        paths.reserve(locations.size());
        for (const Location &location : locations)
        {
            paths.push_back(location.virtualPath());
        }
        return paths;
    }

}  // namespace platform::vfs
