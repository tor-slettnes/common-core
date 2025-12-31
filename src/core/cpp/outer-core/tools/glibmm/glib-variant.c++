/// -*- c++ -*-
//==============================================================================
/// @file glib-variant.c++
/// @brief Wrappers/containers for Glib::VariantBase
/// @author Tor Slettnes
//==============================================================================

#include "glib-variant.h++"

namespace Glib
{
    std::ostream &operator<<(std::ostream &stream, const VariantBase &obj)
    {
        if (obj)
        {
            stream << obj.print();
        }
        else
        {
            stream << "{}";
        }
        return stream;
    }

}  // namespace Glib
