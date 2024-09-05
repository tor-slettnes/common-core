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
    // VFS Path

    Path::Path(const ContextName &context, const fs::path &relpath)
        : context(context),
          relpath(relpath)
    {
    }

    void Path::check_relative(const ContextName &context,
                              const fs::path &relpath,
                              const fs::path &abspath)
    {
        if (relpath.is_absolute())
        {
            throw fs::filesystem_error(
                "Absolute path not allowed in virtual context",
                context + ":" + relpath.string(),
                abspath,
                std::make_error_code(std::errc::no_such_file_or_directory));
        }
    }

    void Path::to_literal_stream(std::ostream &stream) const
    {
        stream << "\""
               << this->context
               << ":"
               << this->relpath.string()
               << "\"";
    }


    void Path::to_stream(std::ostream &stream) const
    {
        stream << this->context
               << ":"
               << this->relpath.string();
    }

    Path::operator bool() const noexcept
    {
        return this->context.size();
    }

    Path &Path::operator/=(const fs::path &relpath)
    {
        Path::check_relative(this->context, this->relpath / relpath);
        this->relpath /= relpath;
        return *this;
    }

    Path &Path::operator+=(const fs::path &relpath)
    {
        Path::check_relative(this->context, this->relpath / relpath);
        this->relpath += relpath;
        return *this;
    }

    bool Path::operator==(const Path &other) const
    {
        return (this->context == other.context) &&
               (this->relpath == other.relpath);
    }

    Path operator/(const Path &lhs, const fs::path &rhs)
    {
        Path::check_relative(lhs.context, lhs.relpath / rhs);
        return {lhs.context, lhs.relpath / rhs};
    }

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
        fs::path provided = fs::weakly_canonical(this->root / relpath);

        auto it1 = jail.begin();
        auto it2 = provided.begin();

        while ((it1 != jail.end()) && (it2 != provided.end()) &&
               (*it1 == *it2))
        {
            it1++, it2++;
        }

        if (it1 != jail.end())
        {
            throw fs::filesystem_error(
                "Virtual path would fall outside context root",
                this->name + ":" + relpath.string(),
                provided,
                std::make_error_code(std::errc::no_such_file_or_directory));
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
    // platform::vfs::ContextProxy methods

    Location::Location()
        : ContextProxy()
    {
    }

    Location::Location(Context::ptr ref,
                       const fs::path &relpath,
                       bool modify)
        : ContextProxy(ref, modify),
          relpath(relpath)
    {
        Path::check_relative(this->context->name, relpath);
    }

    Location::Location(Context::ptr ref,
                       bool modify)
        : ContextProxy(ref, modify)
    {
    }

    Location::Location(const Location &other)
        : ContextProxy(other),
          relpath(other.relpath)
    {
    }

    fs::path Location::localPath() const
    {
        if (this->context)
        {
            return this->context->localPath(this->relpath);
        }
        else
        {
            return {};
        }
    }

    fs::path Location::localPath(const fs::path &relpath) const
    {
        if (this->context)
        {
            return this->context->localPath(this->relpath / relpath);
        }
        else
        {
            return {};
        }
    }

    Path Location::virtualPath() const
    {
        return this->context->virtualPath(this->relpath);
    }

    Path Location::virtualPath(const fs::path &relpath) const
    {
        return this->context->virtualPath(this->relpath / relpath);
    }

    void Location::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                          "Location(\"%s:%s\", modify=%b)",
                          this->context->name,
                          this->relpath.string(),
                          this->modify);
    }

    //========================================================================
    // Output stream representations

    std::ostream &operator<<(std::ostream &stream, const FileStats &stats)
    {
        core::types::PartsList parts;
        parts.add("type", stats.type, true);
        parts.add("size", stats.size, true, "%'d");
        parts.add("link", stats.link, !stats.link.empty(), "%r");
        parts.add("mode", stats.mode, true, "0%03o");
        parts.add("readable", stats.readable, true, "%b");
        parts.add("writable", stats.writable, true, "%b");
        parts.add("uid", stats.uid, true, "%d");
        parts.add("gid", stats.gid, true, "%d");
        parts.add("owner", stats.owner, true, "%r");
        parts.add("group", stats.group, true, "%r");
        parts.add_value("accessTime", stats.accessTime, true, "%s");
        parts.add_value("modifyTime", stats.modifyTime, true, "%s");
        parts.add_value("createTime", stats.createTime, true, "%s");
        parts.add_value("attributes", stats.attributes, true, "%s");
        stream << parts;
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const VolumeStats &stats)
    {
        core::types::PartsList parts;
        parts.add("capacity", stats.capacity, true, "%'d");
        parts.add("free", stats.free, true, "%'d");
        parts.add("available", stats.available, true, "%'d");
        stream << parts;
        return stream;
    }
}  // namespace platform::vfs
