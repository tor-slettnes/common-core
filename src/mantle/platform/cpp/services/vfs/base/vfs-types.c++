// -*- c++ -*-
//==============================================================================
/// @file vfs-types.c++
/// @brief VFS service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-types.h++"
#include "types/partslist.h++"

namespace platform::vfs
{
    //==========================================================================
    /// Operational flags for copy(), move(), remove(), create_folder()

    std::ostream &operator<<(std::ostream &stream, const OperationFlags &flags)
    {
        std::vector<std::string> strings = {};
        if (flags.inside_target)
        {
            strings.push_back("inside_target");
        }

        if (flags.force)
        {
            strings.push_back("force");
        }

        if (flags.dereference)
        {
            strings.push_back("dereference");
        }

        if (flags.merge)
        {
            strings.push_back("merge");
        }

        if (flags.update)
        {
            strings.push_back("update");
        }

        if (flags.with_attributes)
        {
            strings.push_back("with_attributes");
        }

        if (flags.include_hidden)
        {
            strings.push_back("include_hidden");
        }

        if (flags.ignore_case)
        {
            strings.push_back("ignore_case");
        }

        stream << "flags=" << strings;
        return stream;
    }

    //========================================================================
    // Output stream representations

    std::ostream &operator<<(std::ostream &stream, const FileInfo &stats)
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

    //==========================================================================
    // VFS Path

    Path::Path(const ContextName &context, const fs::path &relpath)
        : context(context),
          relpath(relpath)
    {
    }

    Path Path::remove_filename() const
    {
        fs::path relpath = this->relpath;
        relpath.remove_filename();
        return {this->context, relpath};
    }

    fs::path Path::filename() const
    {
        return this->relpath.filename();
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
}  // namespace platform::vfs

namespace std::filesystem
{
    std::ostream &operator<<(std::ostream &stream, const space_info &info)
    {
        core::types::PartsList parts;
        parts.add("capacity", info.capacity, true, "%'d");
        parts.add("free", info.free, true, "%'d");
        parts.add("available", info.available, true, "%'d");
        stream << parts;
        return stream;
    }

}  // namespace std::filesystem
