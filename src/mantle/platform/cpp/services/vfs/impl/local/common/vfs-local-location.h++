// -*- c++ -*-
//==============================================================================
/// @file vfs-local-location.h++
/// @brief VFS service - local filesytem location handle
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-context.h++"

namespace platform::vfs::local
{
    //==========================================================================
    // LocalLocation

    class LocalLocation : public Location
    {
        using This = LocalLocation;
        using Super = Location;

    public:
        using Super::Super;

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

    protected:
        virtual FileStats read_stats(
            const fs::path &localpath,
            bool dereference) const;

        virtual void copy_from(
            const Location &source,
            const OperationFlags &flags) const;

        virtual void move_from(
            const Location &source,
            const OperationFlags &flags) const;

        virtual void remove(
            const Location &loc,
            const OperationFlags &flags) const;

        virtual void locate_inside(
            const fs::path &root,
            const fs::path &relpath,
            const std::vector<std::string> &filename_masks,
            const core::types::TaggedValueList &attribute_filters,
            bool with_attributes,
            bool include_hidden,
            bool ignore_case,
            Directory *dir) const;

        virtual bool filename_match(
            const std::vector<std::string> &masks,
            const fs::path &basename,
            bool include_hidden,
            bool ignore_case) const;

        virtual bool attribute_match(
            const core::types::TaggedValueList &attribute_filters,
            const core::types::KeyValueMap &attributes) const;

        std::shared_ptr<AttributeStore> attribute_store(
            fs::file_type type_hint = fs::file_type::none) const;

    private:
        std::shared_ptr<AttributeStore> attribute_store_;
    };
}  // namespace platform::vfs::local
