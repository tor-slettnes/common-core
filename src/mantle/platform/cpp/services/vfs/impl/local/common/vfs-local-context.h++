// -*- c++ -*-
//==============================================================================
/// @file vfs-local-context.h++
/// @brief VFS service - local system context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-context.h++"

#include <mutex>

namespace platform::vfs::local
{
    //==========================================================================
    // LocalContext

    class LocalContext : public Context,
                         public std::enable_shared_from_this<LocalContext>
    {
    public:
        LocalContext(const std::string &name,
                     const fs::path &root,
                     bool writable,
                     const std::string &preexec,
                     const std::string &postexec,
                     const std::string &title,
                     bool removable = false);

        void add_ref() override;
        void del_ref() override;

        void open();
        void close();

    protected:
        virtual void on_open();
        virtual void on_close();

    public:
        std::string preexec;
        std::string postexec;
        std::uint64_t refcount;

    private:
        std::mutex mtx;
    };

    // //==========================================================================
    // // LocalLocation

    // class LocalLocation : public Location
    // {
    //     using This = LocalLocation;
    //     using Super = Location;

    // public:
    //     using Super::Super;

    // public:
    //     VolumeStats volume_stats(
    //         const fs::path &relpath) const override;

    //     FileStats file_stats(
    //         const fs::path &relpath,
    //         bool dereference = false,
    //         bool with_attributes = false) const override;

    //     Directory get_directory(
    //         const fs::path &relpath,
    //         const OperationFlags &flags = {}) const override;

    //     Directory locate(
    //         const fs::path &relpath,
    //         const std::vector<std::string> &filename_masks,
    //         const core::types::TaggedValueList &attribute_filters = {},
    //         const OperationFlags &flags = {}) const override;

    //     void copy_from(
    //         const LocationList &sources,
    //         const fs::path &relpath,
    //         const OperationFlags &flags = {}) const override;

    //     void move_from(
    //         const LocationList &sources,
    //         const fs::path &relpath,
    //         const OperationFlags &flags = {}) const override;

    //     void remove(
    //         const fs::path &relpath,
    //         const OperationFlags &flags = {}) const override;

    //     void create_folder(
    //         const fs::path &relpath,
    //         const OperationFlags &flags) const override;

    //     UniqueReader read_file(
    //         const fs::path &relpath) const override;

    //     UniqueWriter write_file(
    //         const fs::path &relpath) const override;

    //     core::types::KeyValueMap get_attributes(
    //         const fs::path &relpath) const override;

    //     void set_attributes(
    //         const fs::path &relpath,
    //         const core::types::KeyValueMap &attributes) const override;

    //     void clear_attributes(
    //         const fs::path &relpath) const override;

    // protected:
    //     virtual void copy2(
    //         const Location &srcloc,
    //         const Location &tgtloc,
    //         const OperationFlags &flags) const;

    //     virtual void move2(
    //         const Location &srcloc,
    //         const Location &tgtloc,
    //         const OperationFlags &flags) const;

    //     virtual void remove(
    //         const Location &loc,
    //         const OperationFlags &flags) const;

    //     virtual FileStats read_stats(
    //         const fs::path &localpath,
    //         bool dereference = false) const;

    //     virtual void locate_inside(
    //         const fs::path &root,
    //         const fs::path &relpath,
    //         const std::vector<std::string> &filename_masks,
    //         const core::types::TaggedValueList &attribute_filters,
    //         bool with_attributes,
    //         bool include_hidden,
    //         bool ignore_case,
    //         Directory *dir) const;

    //     virtual bool filename_match(
    //         const std::vector<std::string> &masks,
    //         const fs::path &basename,
    //         bool include_hidden,
    //         bool ignore_case) const;

    //     virtual bool attribute_match(
    //         const core::types::TaggedValueList &attribute_filters,
    //         const core::types::KeyValueMap &attributes) const;

    //     virtual core::types::KeyValueMap get_attributes(
    //         const fs::path &localpath,
    //         fs::file_type type_hint = fs::file_type::none) const;

    //     virtual void set_attributes(
    //         const fs::path &localpath,
    //         const core::types::KeyValueMap &attributes,
    //         fs::file_type pathtype = fs::file_type::none,
    //         bool save = true) const;

    //     virtual void clear_attributes(
    //         const fs::path &localpath,
    //         fs::file_type type_hint = fs::file_type::none,
    //         bool save = true) const;

    // };
}  // namespace platform::vfs::local
