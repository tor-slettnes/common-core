/// -*- c++ -*-
//==============================================================================
/// @file python-containerobject.h++
/// @brief Data access for Python objects with exposed attributes
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "python-object.h++"

namespace cc::python
{
    class ContainerObject : public Object
    {
    public:
        // Inherit constructors
        using Object::Object;

        /// @brief
        ///    Obtain attribute names within this container instance.
        /// @return
        ///    Attribute names contained in this instance
        std::vector<std::string> dir() const;
        Object getattr(const std::string& name) const;

        Map attributes_as_objects() const;
        types::KeyValueMap attributes_as_values() const;

    protected:
    };
}  // namespace cc::python
