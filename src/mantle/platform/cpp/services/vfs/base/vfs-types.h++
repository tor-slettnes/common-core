// -*- c++ -*-
//==============================================================================
/// @file vfs-types.h++
/// @brief VFS service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logging/logging.h++"
#include "types/filesystem.h++"

#include <iostream>

namespace platform::vfs
{
    define_log_scope("vfs");

    using ReaderRef = std::unique_ptr<std::istream>;
    using WriterRef = std::unique_ptr<std::ostream>;

    /// Flags for copy(), move(), remove(), createFolder()
    struct OperationFlags
    {
        bool force = false;
        bool dereference = false;
        bool merge = false;
        bool update = false;
        bool with_attributes = false;
        bool include_hidden = false;
        bool ignore_case = false;
        bool inside_target = false;
    };

    std::ostream &operator<<(std::ostream &stream, const OperationFlags &flags);
}
