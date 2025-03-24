// -*- c++ -*-
//==============================================================================
/// @file vfs-location.c++
/// @brief VFS service - abstract file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-location.h++"
#include "status/exceptions.h++"
#include "platform/path.h++"

namespace vfs
{
    //==========================================================================
    // ContextProxy

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
        if (modify && this->context && !this->context->writable)
        {
            throwf(core::exception::PermissionDenied,
                   "No wrire access in file context %r",
                   this->context->name);
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

    fs::path ContextProxy::localRoot() const
    {
        return this->localPath();
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

    //==========================================================================
    // Location

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

    fs::path Location::localPath() const
    {
        return Super::localPath(this->relpath);
    }

    fs::path Location::localPath(const fs::path &relpath) const
    {
        return Super::localPath(this->relpath / relpath);
    }

    Path Location::virtualPath() const
    {
        return Super::virtualPath(this->relpath);
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

}  // namespace vfs
