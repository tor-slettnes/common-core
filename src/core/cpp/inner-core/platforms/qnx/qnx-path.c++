/// -*- c++ -*-
//==============================================================================
/// @file qnx-path.c++
/// @brief Path-related functions - QNX specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "qnx-path.h++"

namespace core::platform
{
    QNXPathProvider::QNXPathProvider(const std::string &exec_path)
        : Super("QNXPathProvider", exec_path)
    {
    }

    fs::path QNXPathProvider::exec_path() const noexcept
    {
        fs::path path = this->readtext("/proc/self/exefile");
        return path.empty() ? Super::exec_path() : path;
    }

}  // namespace core::platform
