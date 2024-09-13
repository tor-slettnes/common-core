// -*- c++ -*-
//==============================================================================
/// @file vfs-remote-location.h++
/// @brief VFS service - remote filesytem location handle
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-grpc-client.h++"
#include "vfs-location.h++"

namespace platform::vfs
{
    //==========================================================================
    // RemoteLocation

    class RemoteLocation : public Location
    {
        using This = RemoteLocation;
        using Super = Location;

    public:
        RemoteLocation(const std::shared_ptr<Client> &client);

    public:
        VolumeStats volume_stats(
            const OperationFlags &flags = {}) const override;

        FileStats file_stats(
            const OperationFlags &flags = {}) const override;

        Directory list_folder(
            const OperationFlags &flags = {}) const override;

        Directory locate(
            const std::vector<std::string> &filename_masks,
            const core::types::TaggedValueList &attribute_filters = {},
            const OperationFlags &flags = {}) const override;

        void copy_from(
            const LocationList &sources,
            const OperationFlags &flags = {}) const override;

        void move_from(
            const LocationList &sources,
            const OperationFlags &flags = {}) const override;

        void remove(
            const OperationFlags &flags = {}) const override;

        void create_folder(
            const OperationFlags &flags) const override;

        UniqueReader read() const override;

        UniqueWriter write() const override;

        core::types::KeyValueMap get_attributes() const override;

        void set_attributes(
            const core::types::KeyValueMap &attributes) const override;

        void clear_attributes() const override;

    private:
        PathList location_paths(const LocationList &locations);

    private:
        std::shared_ptr<Client> client;
    };
}  // namespace platform::vfs
