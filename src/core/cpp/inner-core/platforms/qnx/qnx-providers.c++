/// -*- c++ -*-
//==============================================================================
/// @file qnx-providers.c++
/// @brief Superset of QNX platform providers
/// @author Tor Slettnes
//==============================================================================

#include "posix-providers.h++"
#include "qnx-symbols.h++"
#include "qnx-path.h++"

namespace cc::core::platform
{
    void register_qnx_providers(const fs::path &exec_path)
    {
        path.registerProvider<QNXPathProvider>(exec_path);
    }

    void unregister_qnx_providers()
    {
        path.unregisterProvider<QNXPathProvider>();
    }

    void register_providers(const fs::path &exec_path)
    {
        register_posix_providers(exec_path);
        register_qnx_providers(exec_path);
    }

    void unregister_providers()
    {
        unregister_qnx_providers();
        unregister_posix_providers();
    }
}  // namespace cc::core::platform
