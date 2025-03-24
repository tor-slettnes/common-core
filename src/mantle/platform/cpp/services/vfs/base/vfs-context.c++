// -*- c++ -*-
//==============================================================================
/// @file vfs-context.c++
/// @brief VFS service - abstract file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-context.h++"
#include "status/exceptions.h++"

namespace vfs
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

}  // namespace vfs
