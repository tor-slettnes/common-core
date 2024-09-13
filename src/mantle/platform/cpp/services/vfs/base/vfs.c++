// -*- c++ -*-
//==============================================================================
/// @file vfs.c++
/// @brief VFS service - common API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs.h++"
#include <fstream>

namespace platform::vfs
{
    //==========================================================================
    // Public API


    fs::path localPath(const Path &vpath)
    {
        return get_context(vpath.context)->localPath(vpath.relpath);
    }

    Path getParentPath(const Path &vpath)
    {
        fs::path tempPath = vpath.relpath;
        tempPath.remove_filename();
        Path::check_relative(vpath.context, tempPath);
        return Path(vpath.context, tempPath);
    }

    ContextList listContexts()
    {
        ContextMap map = get_contexts();
        ContextList list;
        list.reserve(map.size());
        for (const auto &[name, ref] : map)
            list.push_back(ref);
        return list;
    }

    ContextMap get_contexts(bool removable_only, bool open_only)
    {
        ContextMap contexts = open_only
                                  ? vfs->get_open_contexts()
                                  : vfs->get_contexts();

        if (removable_only)
        {
            auto it = contexts.begin();
            while (it != contexts.end())
            {
                if (it->second->removable)
                {
                    it++;
                }
                else
                {
                    it = contexts.erase(it);
                }
            }
        }
        return contexts;
    }

    ContextMap get_open_context(bool removable)
    {
        return get_contexts(removable, true);
    }

    ContextMap getRemovableContexts()
    {
        return get_contexts(true);
    }

    Context::ptr get_context(const std::string &name, bool required)
    {
        return vfs->get_context(name, required);
    }

    Context::ptr open_context(const std::string &name, bool required)
    {
        return vfs->open_context(name, required);
    }

    void close_context(const Context::ptr &cxt)
    {
        return vfs->close_context(cxt);
    }

    void close_context(const std::string &name)
    {
        if (auto cxt = vfs::get_context(name, false))
        {
            vfs->close_context(cxt);
        }
    }

    bool exists(const Path &vpath)
    {
        return (vfs::file_stats(vpath, false).type != fs::file_type::none);
    }

    VolumeStats volume_stats(const Path &vpath)
    {
        return vfs->volume_stats(vpath, {});
    }

    FileStats file_stats(const Path &vpath,
                         bool with_attributes,
                         bool dereference)
    {
        return vfs->file_stats(
            vpath,
            {
                .dereference = dereference,
                .with_attributes = with_attributes,
            });
    }

    Directory get_directory(const Path &vpath,
                            bool with_attributes,
                            bool dereference,
                            bool include_hidden)
    {
        return vfs->get_directory(
            vpath,
            {
                .dereference = dereference,
                .with_attributes = with_attributes,
                .include_hidden = include_hidden,
            });
    }

    DirectoryList list(const Path &vpath,
                       bool with_attributes,
                       bool dereference,
                       bool include_hidden)
    {
        Directory dir = get_directory(
            vpath,
            with_attributes,
            dereference,
            include_hidden);

        return DirectoryList(dir.begin(), dir.end());
    }

    Directory locate(
        const Path &root,
        const std::vector<std::string> &filename_masks,
        const core::types::TaggedValueList &attribute_filters,
        bool with_attributes,
        bool include_hidden,
        bool ignore_case)
    {
        return vfs->locate(
            root,
            filename_masks,
            attribute_filters,
            {
                .with_attributes = with_attributes,
                .include_hidden = include_hidden,
                .ignore_case = ignore_case,
            });
    }

    void copy(const Path &source,
              const Path &target,
              bool force,
              bool dereference,
              bool merge,
              bool update,
              bool with_attributes,
              bool inside_target)
    {
        return vfs->copy(
            {source},
            target,
            {
                .force = force,
                .dereference = dereference,
                .merge = merge,
                .update = update,
                .with_attributes = with_attributes,
                .inside_target = inside_target,
            });
    }

    void copy(const Paths &sources,
              const Path &target,
              bool force,
              bool dereference,
              bool merge,
              bool update,
              bool with_attributes)
    {
        return vfs->copy(
            sources,
            target,
            {
                .force = force,
                .dereference = dereference,
                .merge = merge,
                .update = update,
                .with_attributes = with_attributes,
                .inside_target = true,
            });
    }

    void move(const Path &source,
              const Path &target,
              bool force,
              bool with_attributes,
              bool inside_target)
    {
        return vfs->move(
            {source},
            target,
            {
                .force = force,
                .with_attributes = with_attributes,
                .inside_target = inside_target,
            });
    }

    void move(const Paths &sources,
              const Path &target,
              bool force,
              bool with_attributes)
    {
        return vfs->move(
            sources,
            target,
            {
                .force = force,
                .with_attributes = with_attributes,
                .inside_target = true,
            });
    }

    void remove(const Path &vpath,
                bool force,
                bool with_attributes)
    {
        return vfs->remove(
            vpath,
            {
                .force = force,
                .with_attributes = with_attributes,
            });
    }

    void create_folder(const Path &vpath,
                       bool force)
    {
        return vfs->create_folder(
            vpath,
            {
                .force = force,
            });
    }

    UniqueReader read_file(const Path &vpath)
    {
        return vfs->read_file(vpath);
    }

    UniqueWriter write_file(const Path &vpath)
    {
        return vfs->write_file(vpath);
    }

    void download(const Path &remote,
                  const fs::path &local)
    {
        auto istream = vfs->read_file(remote);
        auto ostream = std::make_unique<std::ofstream>(local);

        ostream->setstate(std::ios::failbit);
        (*ostream) << istream->rdbuf();
    }

    void upload(const fs::path &local,
                const Path &remote)
    {
        auto istream = std::make_unique<std::ifstream>(local);
        auto ostream = vfs->write_file(remote);

        (*ostream) << istream->rdbuf();
    }

    core::types::KeyValueMap get_attributes(const Path &vpath)
    {
        return vfs->get_attributes(vpath);
    }

    core::types::Value getAttribute(const Path &vpath,
                                    const std::string attribute,
                                    const core::types::Value fallback)
    {
        core::types::KeyValueMap attributes = vfs::get_attributes(vpath);
        return attributes.get(attribute, fallback);
    }

    void set_attributes(const Path &vpath,
                        const core::types::KeyValueMap &attributes)
    {
        return vfs->set_attributes(vpath, attributes);
    }

    void setAttribute(const Path &vpath,
                      const std::string &key,
                      const core::types::Value &value)
    {
        return vfs->set_attributes(vpath, {{key, value}});
    }

    void clear_attributes(const Path &vpath)
    {
        return vfs->clear_attributes(vpath);
    }
}  // namespace platform::vfs
