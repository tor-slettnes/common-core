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

    ContextProxy contextProxy(const std::string &name, bool modify)
    {
        return vfs::ContextProxy(vfs->getContext(name), modify);
    }

    ContextProxy contextProxy(ContextRef ref, bool modify)
    {
        return vfs::ContextProxy(ref, modify);
    }

    Location location(const Path &vpath, bool modify)
    {
        return vfs::location(vpath.context, vpath.relpath, modify);
    }

    Location location(const std::string &context,
                      const fs::path &relpath,
                      bool modify)
    {
        ContextRef cxt = vfs->getContext(context);
        return vfs::Location(cxt, relpath, modify);
    }

    Location location(ContextRef ref, const fs::path &relpath, bool modify)
    {
        return vfs::Location(ref, relpath, modify);
    }

    LocationList locations(const Paths &vpaths, bool modify)
    {
        LocationList locations;
        locations.reserve(vpaths.size());
        for (const auto &vpath : vpaths)
        {
            locations.emplace_back(vfs::location(vpath, modify));
        }
        return locations;
    }

    fs::path localPath(const Path &vpath)
    {
        return getContext(vpath.context)->localPath(vpath.relpath);
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
        ContextMap map = getContexts();
        ContextList list;
        list.reserve(map.size());
        for (const auto &[name, ref] : map)
            list.push_back(ref);
        return list;
    }

    ContextMap getContexts(bool removable_only, bool open_only)
    {
        ContextMap contexts = open_only
                                  ? vfs->getOpenContexts()
                                  : vfs->getContexts();

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

    ContextMap getOpenContexts(bool removable)
    {
        return getContexts(removable, true);
    }

    ContextMap getRemovableContexts()
    {
        return getContexts(true);
    }

    ContextRef getContext(const std::string &name, bool required)
    {
        return vfs->getContext(name, required);
    }

    ContextRef openContext(const std::string &name, bool required)
    {
        return vfs->openContext(name, required);
    }

    void closeContext(const ContextRef &cxt)
    {
        return vfs->closeContext(cxt);
    }

    void closeContext(const std::string &name)
    {
        if (auto cxt = vfs::getContext(name, false))
        {
            vfs->closeContext(cxt);
        }
    }

    bool exists(const Path &vpath)
    {
        return (vfs::fileStats(vpath, false).type != fs::file_type::none);
    }

    VolumeStats volumeStats(const Path &vpath)
    {
        return vfs->volumeStats(vpath, {});
    }

    FileStats fileStats(const Path &vpath,
                        bool with_attributes,
                        bool dereference)
    {
        return vfs->fileStats(
            vpath,
            {
                .dereference = dereference,
                .with_attributes = with_attributes,
            });
    }

    Directory getDirectory(const Path &vpath,
                           bool with_attributes,
                           bool dereference,
                           bool include_hidden)
    {
        return vfs->getDirectory(
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
        Directory dir = getDirectory(
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

    void remove(const Path &source,
                bool force,
                bool with_attributes)
    {
        return remove(Paths({source}), force, with_attributes);
    }

    void remove(const Paths &sources,
                bool force,
                bool with_attributes)
    {
        return vfs->remove(
            sources,
            {
                .force = force,
                .with_attributes = with_attributes,
            });
    }

    void createFolder(const Path &vpath,
                      bool force)
    {
        return vfs->createFolder(
            vpath,
            {
                .force = force,
            });
    }

    ReaderRef readFile(const Path &vpath)
    {
        return vfs->readFile(vpath);
    }

    WriterRef writeFile(const Path &vpath)
    {
        return vfs->writeFile(vpath);
    }

    void download(const Path &remote,
                  const fs::path &local)
    {
        auto istream = vfs->readFile(remote);
        auto ostream = std::make_unique<std::ofstream>(local);

        ostream->setstate(std::ios::failbit);
        (*ostream) << istream->rdbuf();
    }

    void upload(const fs::path &local,
                const Path &remote)
    {
        auto istream = std::make_unique<std::ifstream>(local);
        auto ostream = vfs->writeFile(remote);

        (*ostream) << istream->rdbuf();
    }

    core::types::KeyValueMap getAttributes(const Path &vpath)
    {
        return vfs->getAttributes(vpath);
    }

    core::types::Value getAttribute(const Path &vpath,
                                    const std::string attribute,
                                    const core::types::Value fallback)
    {
        core::types::KeyValueMap attributes = vfs::getAttributes(vpath);
        return attributes.get(attribute, fallback);
    }

    void setAttributes(const Path &vpath,
                       const core::types::KeyValueMap &attributes)
    {
        return vfs->setAttributes(vpath, attributes);
    }

    void setAttribute(const Path &vpath,
                      const std::string &key,
                      const core::types::Value &value)
    {
        return vfs->setAttributes(vpath, {{key, value}});
    }

    void clearAttributes(const Path &vpath)
    {
        return vfs->clearAttributes(vpath);
    }
}  // namespace platform::vfs
