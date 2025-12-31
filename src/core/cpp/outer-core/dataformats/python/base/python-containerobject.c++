/// -*- c++ -*-
//==============================================================================
/// @file python-containerobject.c++
/// @brief Data access for Python objects with exposed attributes
/// @author Tor Slettnes
//==============================================================================

#include "python-containerobject.h++"
#include "status/exceptions.h++"
#include "string/misc.h++"

namespace core::python
{
    std::string ContainerObject::name() const
    {
        if (ContainerObject name_obj = this->getattr("__name__"))
        {
            return name_obj.as_string().value_or("");
        }
        else
        {
            return "(unnamed)";
        }
    }

    std::vector<std::string> ContainerObject::dir() const
    {
        std::vector<std::string> symbols;
        if (this->cobj)
        {
            SimpleObject dirlist = PyObject_Dir(this->cobj);
            Py_ssize_t size = PyList_Size(dirlist.borrow());
            symbols.reserve(size);
            for (uint c = 0; c < size; c++)
            {
                SimpleObject object(PyList_GetItem(dirlist.borrow(), c), true);
                symbols.push_back(object.as_string().value());
            }
        }
        return symbols;
    }

    ContainerObject ContainerObject::getattr(const std::string &name) const
    {
        if (this->cobj)
        {
            SimpleObject py_name(SimpleObject::pystring_from_string(name));
            return PyObject_GetAttr(this->cobj, py_name.borrow());
        }
        else
        {
            return nullptr;
        }
    }

    ContainerObject::Map ContainerObject::attributes_as_objects() const
    {
        Map map;
        for (const std::string &name : this->dir())
        {
            if (ContainerObject obj = this->getattr(name))
            {
                map.insert_or_assign(name, obj);
            }
        }
        return map;
    }

    types::KeyValueMap ContainerObject::attributes_as_kvmap() const
    {
        types::KeyValueMap kvmap;
        for (const auto &[key, obj] : this->attributes_as_objects())
        {
            if (obj.borrow())
            {
                kvmap.insert_or_assign(key, obj.as_value());
            }
        }
        return kvmap;
    }

    ContainerObject ContainerObject::find_qualified_symbol(const std::string &qualified_name) const
    {
        // We do this recursively, because `typeof(this)` may in fact be a
        // subclass with an overridden `getattr()` method.

        std::vector<std::string> parts = str::split(qualified_name, ".", 1);
        if (parts.size() > 0)
        {
            if (ContainerObject obj = this->getattr(parts.front()))
            {
                if (parts.size() > 1)
                {
                    return obj.find_qualified_symbol(parts.back());
                }
                else
                {
                    return obj;
                }
            }
        }

        return nullptr;
    }

}  // namespace core::python
