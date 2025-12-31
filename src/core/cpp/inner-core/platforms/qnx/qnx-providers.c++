/// -*- c++ -*-
//==============================================================================
/// @file qnx-providers.c++
/// @brief Superset of QNX platform providers
/// @author Tor Slettnes
//==============================================================================

#include "posix-providers.h++"
#include "qnx-symbols.h++"
#include "qnx-path.h++"

namespace core::platform
{
    void register_qnx_providers(const std::string &exec_name)
    {
        path.registerProvider<QNXPathProvider>(exec_name);
    }

    void unregister_qnx_providers()
    {
        path.unregisterProvider<QNXPathProvider>();
    }

    void register_providers(const std::string &exec_name)
    {
        register_posix_providers(exec_name);
        register_qnx_providers(exec_name);
    }

    void unregister_providers()
    {
        unregister_qnx_providers();
        unregister_posix_providers();
    }
}  // namespace core::platform
