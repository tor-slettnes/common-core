/// -*- c++ -*-
//==============================================================================
/// @file glib-variant.h++
/// @brief Wrappers/containers for Glib::VariantBase
/// @author Tor Slettnes <tor@slett.net>
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
