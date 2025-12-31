/// -*- c++ -*-
//==============================================================================
/// @file listable.c++
/// @brief Base to represent object as a tagged value lsit
/// @author Tor Slettnes
//==============================================================================

#include "listable.h++"

namespace core::types
{
    Value &operator<<(Value &value, const Listable &obj)
    {
        value = obj.as_tvlist();
        return value;
    }

    Value &operator<<(Value &value, const std::shared_ptr<Listable> &ptr)
    {
        if (ptr)
        {
            value = ptr->as_tvlist();
        }
        else
        {
            value.clear();
        }
        return value;
    }

    ValueList &operator<<(ValueList &list, const Listable &obj)
    {
        list = obj.as_valuelist();
        return list;
    }

    TaggedValueList &operator<<(TaggedValueList &tvlist, const Listable &obj)
    {
        obj.to_tvlist(&tvlist);
        return tvlist;
    }

    KeyValueMap &operator<<(KeyValueMap &kvmap,
                            const Listable &obj)
    {
        kvmap = obj.as_kvmap();
        return kvmap;
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
