// -*- c++ -*-
//==============================================================================
/// @file vfs-types.c++
/// @brief VFS service - data types
/// @author Tor Slettnes
//==============================================================================

#include "vfs-types.h++"

namespace vfs
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
        stream << this->as_tvlist();
    }

    void Path::to_stream(std::ostream &stream) const
    {
        stream << this->context
               << ":"
               << this->relpath.string();
    }

    void Path::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append("context", this->context);
        tvlist->append("relpath", this->relpath.string());
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
}  // namespace vfs

namespace std::filesystem
{
    core::types::TaggedValueList &operator<<(
        core::types::TaggedValueList &tvlist,
        const space_info &volinfo)
    {
        return tvlist.extend({
            {"capacity", volinfo.capacity},
            {"free", volinfo.free},
            {"available", volinfo.available},
        });
    }

    std::ostream &operator<<(std::ostream &stream, const space_info &info)
    {
        return stream << core::types::TaggedValueList::create_from(info);
    }

}  // namespace std::filesystem
