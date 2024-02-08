/// -*- c++ -*-
//==============================================================================
/// @file python-containerobject.c++
/// @brief Data access for Python objects with exposed attributes
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "python-containerobject.h++"

namespace cc::python
{
    std::vector<std::string> ContainerObject::dir() const
    {
        std::vector<std::string> symbols;
        if (this->cobj)
        {
            Object dirlist = PyObject_Dir(this->cobj);
            Py_ssize_t size = PyList_Size(dirlist.borrow());
            symbols.reserve(size);
            for (uint c = 0; c < size; c++)
            {
                Object object(PyList_GetItem(dirlist.borrow(), c), true);
                symbols.push_back(object.as_string().value());
            }
        }
        return symbols;
    }

    Object ContainerObject::getattr(const std::string &name) const
    {
        if (this->cobj)
        {
            Object py_name(Object::pystring_from_string(name));
            return PyObject_GetAttr(this->cobj, py_name.borrow());
        }
        else
        {
            return nullptr;
        }
    }

    Object::Map ContainerObject::attributes_as_objects() const
    {
        Map map;
        for (const std::string &name : this->dir())
        {
            Object obj(this->getattr(name));
            map.insert_or_assign(name, obj);
        }
        return map;
    }

    types::KeyValueMap ContainerObject::attributes_as_values() const
    {
        types::KeyValueMap kvmap;
        for (const std::string &name : this->dir())
        {
            Object obj(this->getattr(name));
            types::Value value(obj.as_value());
            if (value || Py_IsNone(obj.borrow()))
            {
                kvmap.insert_or_assign(name, value);
            }
        }
        return kvmap;
    }

}  // namespace cc::python
