/// -*- c++ -*-
//==============================================================================
/// @file python-simpleobject.c++
/// @brief Generic data access for Python objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "python-simpleobject.h++"

namespace core::python
{
    SimpleObject::SimpleObject(PyObject *cobj, bool borrowed)
        : cobj(cobj)
    {
        if (borrowed)
        {
            Py_XINCREF(this->cobj);
        }
    }

    SimpleObject::SimpleObject(const SimpleObject &other)
        : cobj(other.cobj)
    {
        Py_XINCREF(this->cobj);
    }

    SimpleObject::~SimpleObject()
    {
        Py_XDECREF(this->cobj);
    }

    void SimpleObject::to_stream(std::ostream &stream) const
    {
        SimpleObject::write_to_stream(stream, this->cobj, false);
    }

    void SimpleObject::to_literal_stream(std::ostream &stream) const
    {
        SimpleObject::write_to_stream(stream, this->cobj, true);
    }

    SimpleObject &SimpleObject::operator=(const SimpleObject &other) noexcept
    {
        Py_XDECREF(this->cobj);
        this->cobj = other.cobj;
        Py_INCREF(this->cobj);
        return *this;
    }

    SimpleObject::operator bool() const noexcept
    {
        return this->cobj != nullptr;
    }

    PyObject *SimpleObject::acquire() const
    {
        if (this->cobj)
        {
            Py_INCREF(this->cobj);
        }
        return this->cobj;
    }

    PyObject *SimpleObject::borrow() const
    {
        return this->cobj;
    }

    std::string SimpleObject::type_name() const
    {
#if PY_VERSION_HEX >= 0x030B0000
        if (this->cobj)
        {
            PyObject *type_name = PyType_GetName(Py_TYPE(this->cobj));
            return SimpleObject(type_name).as_string().value();
        }
        else
        {
            return "";
        }
#else
        return "";
#endif
    }

    // types::ValueType SimpleObject::value_type() const
    // {
    //     if (this->cobj)
    //     {
    //         try
    //         {
    //             return SimpleObject::type_map.at(Py_TYPE(this->cobj));
    //         }
    //         catch (const std::out_of_range &)
    //         {
    //         }
    //     }
    //     return types::ValueType::NONE;
    // }

    types::Value SimpleObject::as_value() const
    {
        if (!this->cobj)
        {
            return {};
        }
        else if (PyBool_Check(this->cobj))
        {
            return this->as_bool();
        }
        else if (PyLong_Check(this->cobj))
        {
            if (auto value = this->as_uint())
            {
                return value;
            }
            else
            {
                return this->as_sint();
            }
        }
        else if (PyFloat_Check(this->cobj))
        {
            return this->as_real();
        }
        else if (PyComplex_Check(this->cobj))
        {
            return this->as_complex();
        }
        else if (PyUnicode_Check(this->cobj))
        {
            return this->as_string();
        }
        else if (PyBytes_Check(this->cobj) || PyByteArray_Check(this->cobj))
        {
            return this->as_bytevector();
        }
        else if (PyTuple_Check(this->cobj))
        {
            return this->as_valuelist();
        }
        else if (PyList_Check(this->cobj))
        {
            if (auto tvlist = this->as_tvlist())
            {
                return tvlist;
            }
            else
            {
                return this->as_valuelist();
            }
        }
        else if (PyDict_Check(this->cobj))
        {
            return this->as_kvmap();
        }
        else
        {
            return {};
        }
    }

    std::optional<bool> SimpleObject::as_bool() const
    {
        if (this->cobj && PyBool_Check(this->cobj))
        {
#if PY_VERSION_HEX >= 0x030A0000
            return Py_IsTrue(this->cobj);
#else
            return (this->cobj == Py_True);
#endif
        }
        else
        {
            return {};
        }
    }

    std::optional<types::largest_uint> SimpleObject::as_uint() const
    {
        if (this->cobj && PyLong_Check(this->cobj))
        {
            types::largest_uint value = PyLong_AsUnsignedLongLong(this->cobj);
            if (!PyErr_Occurred())
            {
                return value;
            }
        }
        return {};
    }

    std::optional<types::largest_sint> SimpleObject::as_sint() const
    {
        if (this->cobj && PyLong_Check(this->cobj))
        {
            int overflow = 0;
            types::largest_sint value = PyLong_AsLongLongAndOverflow(this->cobj, &overflow);
            if (!PyErr_Occurred())
            {
                return value;
            }
        }
        return {};
    }

    std::optional<double> SimpleObject::as_real() const
    {
        if (this->cobj && PyFloat_Check(this->cobj))
        {
            return PyFloat_AsDouble(this->cobj);
        }
        else
        {
            return {};
        }
    }

    std::optional<types::complex> SimpleObject::as_complex() const
    {
        if (this->cobj && PyComplex_Check(this->cobj))
        {
            return types::complex(PyComplex_RealAsDouble(this->cobj),
                                  PyComplex_ImagAsDouble(this->cobj));
        }
        else
        {
            return {};
        }
    }

    std::optional<std::string> SimpleObject::as_string() const
    {
        if (this->cobj && PyUnicode_Check(this->cobj))
        {
            if (Py_ssize_t size = 0;
                const char *data = PyUnicode_AsUTF8AndSize(this->cobj, &size))
            {
                return std::string(data, size);
            }
        }
        return {};
    }

    std::optional<types::ByteVector> SimpleObject::as_bytevector() const
    {
        char *bytes = nullptr;
        Py_ssize_t size = 0;

        if (this->cobj && PyBytes_Check(this->cobj))
        {
            bytes = PyBytes_AsString(this->cobj);
            size = PyBytes_Size(this->cobj);
        }
        else if (this->cobj && PyByteArray_Check(this->cobj))
        {
            bytes = PyByteArray_AsString(this->cobj);
            size = PyByteArray_Size(this->cobj);
        }

        if (bytes)
        {
            return types::ByteVector(bytes, bytes + size);
        }
        else
        {
            return {};
        }
    }

    std::optional<types::ValueList> SimpleObject::as_valuelist() const
    {
        if (this->cobj && PyList_Check(this->cobj))
        {
            types::ValueList values;
            Py_ssize_t size = PyList_Size(this->cobj);

            for (int c = 0; c < size; c++)
            {
                values.push_back(SimpleObject(PyList_GetItem(this->cobj, c), true).as_value());
            }
            return values;
        }
        else
        {
            return {};
        }
    }

    std::optional<types::TaggedValueList> SimpleObject::as_tvlist() const
    {
        if (this->cobj && PyList_Check(this->cobj))
        {
            types::TaggedValueList tvlist;
            Py_ssize_t size = PyList_Size(this->cobj);

            for (int c = 0; c < size; c++)
            {
                PyObject *item = PyList_GetItem(this->cobj, c);
                if (PyTuple_Check(item) && (PyTuple_Size(item) == 2))
                {
                    types::Tag tag;
                    PyObject *tag_obj = PyTuple_GetItem(item, 0);
                    if (PyUnicode_Check(tag_obj))
                    {
                        tag = SimpleObject(tag_obj, true).as_string();
                    }
#if PY_VERSION_HEX >= 0x030A0000
                    else if (!Py_IsNone(tag_obj))
#else
                    else if (tag_obj != Py_None)
#endif
                    {
                        return {};
                    }

                    PyObject *value_obj = PyTuple_GetItem(item, 1);
                    types::Value value = SimpleObject(value_obj, true).as_value();

                    tvlist.emplace_back(tag, value);
                }
                else
                {
                    return {};
                }
            }
            return tvlist;
        }
        else
        {
            return {};
        }
    }

    std::optional<types::KeyValueMap> SimpleObject::as_kvmap() const
    {
        if (this->cobj && PyDict_Check(this->cobj))
        {
            types::KeyValueMap kvmap;
            PyObject *key_obj, *value_obj;
            Py_ssize_t pos = 0;
            while (PyDict_Next(this->cobj, &pos, &key_obj, &value_obj))
            {
                if (auto key = SimpleObject(key_obj, true).as_string())
                {
                    types::Value value = SimpleObject(value_obj, true).as_value();
                    kvmap.insert_or_assign(*key, value);
                }
            }
            return kvmap;
        }
        else
        {
            return {};
        }
    }

    PyObject *SimpleObject::pystring_from_string(const std::string &string)
    {
        return PyUnicode_DecodeUTF8(string.data(), string.size(), nullptr);
    }

    PyObject *SimpleObject::pybytes_from_bytes(const types::ByteVector &bytes)
    {
        return PyBytes_FromStringAndSize(
            reinterpret_cast<const char *>(bytes.data()),
            bytes.size());
    }

    PyObject *SimpleObject::pytuple_from_values(const types::ValueList &values)
    {
        PyObject *tuple = PyTuple_New(values.size());
        for (uint c = 0; c < values.size(); c++)
        {
            PyTuple_SET_ITEM(tuple, c, SimpleObject::pyobj_from_value(values.at(c)));
        }
        return tuple;
    }

    PyObject *SimpleObject::pytuple_from_objects(const Vector &objects)
    {
        PyObject *tuple = PyTuple_New(objects.size());
        for (uint c = 0; c < objects.size(); c++)
        {
            // PyList_SET_ITEM steals reference, so acquire a new one.
            PyTuple_SET_ITEM(tuple, c, objects.at(c).acquire());
        }
        return tuple;
    }

    PyObject *SimpleObject::pylist_from_values(const types::ValueList &values)
    {
        PyObject *list = PyList_New(values.size());
        for (uint c = 0; c < values.size(); c++)
        {
            // PyList_SET_ITEM steals newly allocated value
            PyList_SET_ITEM(list, c, SimpleObject::pyobj_from_value(values.at(c)));
        }
        return list;
    }

    PyObject *SimpleObject::pylist_from_tagged_values(const types::TaggedValueList &tvlist)
    {
        PyObject *list = PyList_New(tvlist.size());
        for (uint c = 0; c < tvlist.size(); c++)
        {
            const auto &[tag, value] = tvlist.at(c);
            PyObject *tag_obj =
                tag ? PyUnicode_DecodeUTF8(tag->data(), tag->size(), nullptr)
                    : (Py_INCREF(Py_None), Py_None);
            PyObject *value_obj = SimpleObject::pyobj_from_value(value);
            PyList_SET_ITEM(list, c, PyTuple_Pack(2, tag_obj, value_obj));
            Py_DECREF(tag_obj);
            Py_DECREF(value_obj);
        }
        return list;
    }

    PyObject *SimpleObject::pydict_from_kvmap(const types::KeyValueMap &kvmap)
    {
        PyObject *dict = PyDict_New();
        for (const auto &[key, value] : kvmap)
        {
            SimpleObject key_obj(PyUnicode_DecodeUTF8(key.data(), key.size(), nullptr));
            SimpleObject value_obj(SimpleObject::pyobj_from_value(value));
            PyDict_SetItem(dict, key_obj.borrow(), value_obj.borrow());
        }
        return dict;
    }

    PyObject *SimpleObject::pydict_from_objects(const Map &kvmap)
    {
        PyObject *dict = PyDict_New();
        for (const auto &[key, value] : kvmap)
        {
            SimpleObject key_obj(PyUnicode_DecodeUTF8(key.data(), key.size(), nullptr));
            PyDict_SetItem(dict, key_obj.borrow(), value.borrow());
        }
        return dict;
    }


    PyObject *SimpleObject::pyobj_from_value(const types::Value &value)
    {
        switch (value.type())
        {
        case types::ValueType::BOOL:
            return PyBool_FromLong(value.as_bool());

        case types::ValueType::CHAR:
            return SimpleObject::pystring_from_string(value.as_string());

        case types::ValueType::UINT:
            return PyLong_FromUnsignedLongLong(value.as_uint());

        case types::ValueType::SINT:
            return PyLong_FromLongLong(value.as_sint());

        case types::ValueType::REAL:
        case types::ValueType::TIMEPOINT:
        case types::ValueType::DURATION:
            return PyFloat_FromDouble(value.as_real());

        case types::ValueType::COMPLEX:
            return PyComplex_FromDoubles(value.as_real(), value.as_imag());

        case types::ValueType::STRING:
            return SimpleObject::pystring_from_string(value.get<std::string>());

        case types::ValueType::BYTEVECTOR:
            return SimpleObject::pybytes_from_bytes(value.get<types::ByteVector>());

        case types::ValueType::VALUELIST:
            return SimpleObject::pylist_from_values(value.get_valuelist());

        case types::ValueType::KVMAP:
            return SimpleObject::pydict_from_kvmap(value.get_kvmap());

        case types::ValueType::TVLIST:
            return SimpleObject::pylist_from_tagged_values(value.get_tvlist());

        default:
            Py_RETURN_NONE;
        }
    }

    std::ostream &SimpleObject::write_to_stream(
        std::ostream &stream,
        PyObject *obj,
        bool literal)
    {
        if (obj)
        {
            PyObject *pystr = literal ? PyObject_Repr(obj) : PyObject_Str(obj);
            Py_ssize_t size = 0;
            const char *data = PyUnicode_AsUTF8AndSize(pystr, &size);
            stream.write(data, size);
            Py_DECREF(pystr);
        }
        else
        {
            stream << "(Empty)";
        }
        return stream;
    }

    // std::unordered_map<PyTypeObject *, types::ValueType> SimpleObject::type_map = {
    //     {&PyBool_Type, types::ValueType::BOOL},
    //     {&PyLong_Type, types::ValueType::SINT},
    //     {&PyFloat_Type, types::ValueType::REAL},
    //     {&PyComplex_Type, types::ValueType::COMPLEX},
    //     {&PyUnicode_Type, types::ValueType::STRING},
    //     {&PyBytes_Type, types::ValueType::BYTEVECTOR},
    //     {&PyByteArray_Type, types::ValueType::BYTEVECTOR},
    //     {&PyList_Type, types::ValueType::VALUELIST},
    //     {&PyTuple_Type, types::ValueType::VALUELIST},
    //     {&PyDict_Type, types::ValueType::KVMAP},
    // };

}  // namespace core::python

std::ostream &operator<<(std::ostream &stream, PyObject *obj)
{
    return core::python::SimpleObject::write_to_stream(stream, obj, false);
}
