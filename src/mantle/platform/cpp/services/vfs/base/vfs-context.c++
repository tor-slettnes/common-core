// -*- c++ -*-
//==============================================================================
/// @file vfs-context.c++
/// @brief VFS service - abstract file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-context.h++"
#include "status/exceptions.h++"
#include "platform/path.h++"

namespace platform::vfs
{

    //==========================================================================
    // Context methods

    Context::Context(const ContextName &name,
                     const fs::path &root,
                     bool writable,
                     bool removable,
                     std::string title)
        : name(name),
          root(root),
          writable(writable),
          removable(removable),
          title(title.empty() ? name : title)
    {
    }

    fs::path Context::localPath(const fs::path &relpath) const
    {
        Path::check_relative(this->name,
                             relpath,
                             this->root / relpath.relative_path());

        this->check_jail(relpath);
        return fs::weakly_canonical(this->root / relpath);
    }

    Path Context::virtualPath(const fs::path &relpath) const
    {
        Path::check_relative(this->name, relpath);
        return {this->name, relpath};
    }

    void Context::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                          "Context(%r, root=%r, writable=%b, removable=%b, title=%r)",
                          this->name,
                          this->root,
                          this->writable,
                          this->removable,
                          this->title);
    }

    void Context::check_jail(const fs::path &relpath) const
    {
        fs::path jail = fs::weakly_canonical(this->root);
        fs::path provided = fs::weakly_canonical(this->root / relpath.relative_path());

        auto it1 = jail.begin();
        auto it2 = provided.begin();

        while ((it1 != jail.end()) && (it2 != provided.end()) &&
               (*it1 == *it2))
        {
            it1++, it2++;
        }

        if (it1 != jail.end())
        {
            throw core::exception::FilesystemError(
                std::make_error_code(std::errc::no_such_file_or_directory).value(),
                this->name + ":" + relpath.string(),
                provided,
                "Virtual path would fall outside context root");
        }
    }

    //==========================================================================
    // platform::vfs::ContextProxy methods

    ContextProxy::ContextProxy()
        : context({}),
          modify(false)
    {
    }

    ContextProxy::ContextProxy(Context::ptr context, bool modify)
        : context(context),
          modify(modify)
    {
        this->add_ref();
    }

    ContextProxy::ContextProxy(const ContextProxy &other)
        : modify(other.modify)
    {
        this->del_ref();
        this->context = other.context;
        this->add_ref();
    }

    ContextProxy::~ContextProxy()
    {
        this->del_ref();
    }

    ContextProxy::operator bool() const noexcept
    {
        return bool(this->context);
    }

    Context *ContextProxy::operator->() const
    {
        if (this->context)
        {
            return this->context.get();
        }
        else
        {
            throw core::exception::FailedPrecondition("Empty VFS context reference");
        }
    }

    void ContextProxy::add_ref()
    {
        if (this->context)
        {
            this->check_access(this->modify);
            this->context->add_ref();
        }
    }

    void ContextProxy::del_ref()
    {
        if (this->context)
        {
            this->context->del_ref();
        }
    }

    void ContextProxy::check_access(bool modify) const
    {
        if (modify && !this->context->writable)
        {
            throw core::exception::PermissionDenied("No write access in this file context");
        }
    }

    void ContextProxy::check_modify_access() const
    {
        this->check_access(true);
    }

    fs::path ContextProxy::localPath(const fs::path &relpath) const
    {
        return (*this)->localPath(relpath);
    }

    Path ContextProxy::virtualPath(const fs::path &relpath) const
    {
        return (*this)->virtualPath(relpath);
    }

    void ContextProxy::to_stream(std::ostream &stream) const
    {
        if (this->context)
        {
            core::str::format(stream,
                              "ContextProxy(%r, root=%r, modify=%b)",
                              this->context->name,
                              this->context->root,
                              this->modify);
        }
        else
        {
            stream << "ContextProxy()";
        }
    }


    // VolumeStats ContextProxy::volume_stats(
    //     const fs::path &relpath,
    //     const OperationFlags &flags) const
    // {
    //     return fs::space(this->localPath(relpath));
    // }

    // FileStats ContextProxy::file_stats(
    //     const fs::path &relpath,
    //     const OperationFlags &flags) const
    // {
    // }

    // Directory ContextProxy::get_directory(
    //     const fs::path &relpath,
    //     const OperationFlags &flags) const
    // {
    // }

    // Directory ContextProxy::locate(
    //     const fs::path &relpath,
    //     const std::vector<std::string> &filename_masks,
    //     const core::types::TaggedValueList &attribute_filters,
    //     const OperationFlags &flags) const
    // {
    // }

    // void ContextProxy::copy_from(
    //     const ContextProxy &other_context,
    //     const fs::path &other_path,
    //     const fs::path &this_path,
    //     const OperationFlags &flags) const
    // {
    // }

    // void ContextProxy::move_from(
    //     const ContextProxy &other_context,
    //     const fs::path &other_path,
    //     const fs::path &this_path,
    //     const OperationFlags &flags) const
    // {
    // }

    // void ContextProxy::remove(
    //     const fs::path &relpath,
    //     const OperationFlags &flags) const
    // {
    // }

    // void ContextProxy::create_folder(
    //     const fs::path &relpath,
    //     const OperationFlags &flags) const
    // {
    // }

    // UniqueReader ContextProxy::read_file(
    //     const fs::path &relpath) const
    // {
    // }

    // UniqueWriter ContextProxy::write_file(
    //     const fs::path &relpath) const
    // {
    // }

    // core::types::KeyValueMap ContextProxy::get_attributes(
    //     const fs::path &relpath) const
    // {
    // }

    // void ContextProxy::set_attributes(
    //     const fs::path &relpath,
    //     const core::types::KeyValueMap &attributes) const
    // {
    // }

    // void ContextProxy::clear_attributes(
    //     const fs::path &relpath) const
    // {
    // }

    //==========================================================================
    // platform::vfs::ContextProxy methods

    Location::Location()
        : Super()
    {
    }

    Location::Location(Context::ptr ref,
                       const fs::path &relpath,
                       bool modify)
        : Super(ref, modify),
          relpath(relpath)
    {
        Path::check_relative(this->context->name, relpath);
    }

    Location::Location(Context::ptr ref,
                       bool modify)
        : Super(ref, modify)
    {
    }

    Location::Location(const Location &other)
        : Super(other),
          relpath(other.relpath)
    {
    }

    fs::path Location::localPath(const fs::path &relpath) const
    {
        return Super::localPath(this->relpath / relpath);
    }

    Path Location::virtualPath(const fs::path &relpath) const
    {
        return Super::virtualPath(this->relpath / relpath);
    }

    void Location::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                          "Location(\"%s:%s\", modify=%b)",
                          this->context->name,
                          this->relpath.string(),
                          this->modify);
    }

}  // namespace platform::vfs
