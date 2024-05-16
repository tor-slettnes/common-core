/// -*- c++ -*-
//==============================================================================
/// @file python-builtin.h++
/// @brief Data access for Python global scope
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "python-containerobject.h++"

namespace core::python
{
    //==========================================================================
    /// @brief
    ///   Python global scope container

    class Builtin : public ContainerObject
    {
    public:
        Builtin();

        void to_stream(std::ostream &stream) const override;
        void to_literal_stream(std::ostream &stream) const override;
        std::string name() const override;

        std::vector<std::string> dir() const override;
        ContainerObject getattr(const std::string &name) const override;
        Map attributes_as_objects() const override;
    };
}  // namespace core::python
