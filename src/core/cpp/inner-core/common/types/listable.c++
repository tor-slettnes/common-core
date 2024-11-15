/// -*- c++ -*-
//==============================================================================
/// @file listable.c++
/// @brief Base to represent object as a tagged value lsit
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "listable.h++"

namespace core::types
{
    TaggedValueList &operator<<(TaggedValueList &tvlist, const Listable &obj)
    {
        obj.to_tvlist(&tvlist);
        return tvlist;
    }

    void Listable::to_stream(std::ostream &stream) const
    {
        this->as_tvlist().to_stream(stream);
    }

    TaggedValueList Listable::as_tvlist() const
    {
        TaggedValueList tvlist;
        this->to_tvlist(&tvlist);
        return tvlist;
    }

    KeyValueMap Listable::as_kvmap() const
    {
        return this->as_tvlist().as_kvmap();
    }

    ValueList Listable::as_valuelist() const
    {
        return this->as_tvlist().values();
    }

}  // namespace core::types
