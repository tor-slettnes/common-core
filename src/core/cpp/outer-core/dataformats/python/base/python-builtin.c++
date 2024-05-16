/// -*- c++ -*-
//==============================================================================
/// @file python-builtin.h++
/// @brief Data access for Python global scope
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "python-builtin.h++"

namespace cc::python
{
    Builtin::Builtin()
        : ContainerObject(nullptr)
    {
    }

    void Builtin::to_stream(std::ostream &stream) const
    {
        stream << this->name();
    }

    void Builtin::to_literal_stream(std::ostream &stream) const
    {
        stream << this->name();
    }

    std::string Builtin::name() const
    {
        return "__builtins__";
    }

    std::vector<std::string> Builtin::dir() const
    {
        Map object_map(this->attributes_as_objects());
        std::vector<std::string> names;
        names.reserve(object_map.size());
        for (const auto &[name, obj] : object_map)
        {
            names.push_back(name);
        }
        return names;
    }

    ContainerObject Builtin::getattr(const std::string &name) const
    {
        SimpleObject name_obj = SimpleObject::pystring_from_string(name);
        PyObject *globals = PyEval_GetBuiltins();
        PyObject *dict_item = PyDict_GetItem(globals, name_obj.borrow());
        return ContainerObject(dict_item, true);
    }

    SimpleObject::Map Builtin::attributes_as_objects() const
    {
        PyObject *globals = PyEval_GetBuiltins();
        Py_ssize_t pos = 0;
        PyObject *key_obj = nullptr;
        PyObject *value_obj = nullptr;

        Map map;
        while (PyDict_Next(globals, &pos, &key_obj, &value_obj))
        {
            map.try_emplace(
                SimpleObject(key_obj, true).as_string().value(),
                value_obj,
                true);
        }

        return map;
    }
}  // namespace cc::python
