/// -*- c++ -*-
//==============================================================================
/// @file python-containerobject.h++
/// @brief Data access for Python objects with exposed attributes
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "python-simpleobject.h++"

namespace cc::python
{
    //==========================================================================
    /// @brief
    ///   RAII Wrapper for Python C objects with attribute lookups

    class ContainerObject : public SimpleObject
    {
    public:
        // Inherit constructors
        using SimpleObject::SimpleObject;

        virtual std::string name() const;

        /// @brief
        ///    Obtain attribute names within this container instance.
        /// @return
        ///    Attribute names contained in this instance
        virtual std::vector<std::string> dir() const;
        virtual ContainerObject getattr(const std::string& name) const;

        virtual Map attributes_as_objects() const;
        virtual types::KeyValueMap attributes_as_kvmap() const;

        ContainerObject find_qualified_symbol(const std::string &qualified_name) const;

    };
}  // namespace cc::python
