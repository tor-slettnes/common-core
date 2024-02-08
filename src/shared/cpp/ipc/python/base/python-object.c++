/// -*- c++ -*-
//==============================================================================
/// @file python-object.c++
/// @brief Generic data access for Python objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "python-object.h++"

namespace cc::python
{
    Object::Object(PyObject *cobj, bool borrowed)
        : cobj(cobj)
    {
        if (borrowed)
        {
            Py_XINCREF(this->cobj);
        }
    }

    Object::Object(const Object &other)
        : cobj(other.cobj)
    {
        Py_XINCREF(this->cobj);
    }

    Object::~Object()
    {
        Py_XDECREF(this->cobj);
    }

    PyObject *Object::pystring_from_string(const std::string &string)
    {
        return PyUnicode_DecodeUTF8(string.data(), string.size(), nullptr);
    }

    PyObject *Object::pybytes_from_bytes(const types::ByteVector &bytes)
    {
        return PyBytes_FromStringAndSize(
            reinterpret_cast<const char *>(bytes.data()),
            bytes.size());
    }

    PyObject *Object::pytuple_from_values(const types::ValueList &values)
    {
        PyObject *tuple = PyTuple_New(values.size());
        for (uint c = 0; c < values.size(); c++)
        {
            PyTuple_SET_ITEM(tuple, c, Object::pyobj_from_value(values.at(c)));
        }
        return tuple;
    }

    PyObject *Object::pylist_from_values(const types::ValueList &values)
    {
        PyObject *list = PyList_New(values.size());
        for (uint c = 0; c < values.size(); c++)
        {
            PyList_SET_ITEM(list, c, Object::pyobj_from_value(values.at(c)));
        }
        return list;
    }

    PyObject *Object::pylist_from_tagged_values(const types::TaggedValueList &tvlist)
    {
        PyObject *list = PyList_New(tvlist.size());
        for (uint c = 0; c < tvlist.size(); c++)
        {
            const auto &[tag, value] = tvlist.at(c);
            PyObject *tag_obj =
                tag ? PyUnicode_DecodeUTF8(tag->data(), tag->size(), nullptr)
                    : Py_NewRef(Py_None);
            PyObject *value_obj = Object::pyobj_from_value(value);
            PyList_SET_ITEM(list, c, PyTuple_Pack(2, tag_obj, value_obj));
            Py_DECREF(tag_obj);
            Py_DECREF(value_obj);
        }
        return list;
    }

    PyObject *Object::pydict_from_kvmap(const types::KeyValueMap &kvmap)
    {
        PyObject *dict = PyDict_New();
        for (const auto &[key, value] : kvmap)
        {
            PyObject *key_obj = PyUnicode_DecodeUTF8(key.data(), key.size(), nullptr);
            PyObject *value_obj = Object::pyobj_from_value(value);
            PyDict_SetItem(dict, key_obj, value_obj);
            Py_DECREF(key_obj);
            Py_DECREF(value_obj);
        }
        return dict;
    }

    PyObject *Object::pyobj_from_value(const types::Value &value)
    {
        switch (value.type())
        {
        case types::ValueType::BOOL:
            return Py_NewRef(value.as_bool() ? Py_True : Py_False);

        case types::ValueType::CHAR:
            return PyBytes_FromStringAndSize(value.get_if<char>(), 1);

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
            return Object::pystring_from_string(value.get<std::string>());

        case types::ValueType::BYTEVECTOR:
            return Object::pybytes_from_bytes(value.get<types::ByteVector>());

        case types::ValueType::VALUELIST:
            return Object::pylist_from_values(*value.get_valuelist());

        case types::ValueType::KVMAP:
            return Object::pydict_from_kvmap(*value.get_kvmap());

        case types::ValueType::TVLIST:
            return Object::pylist_from_tagged_values(*value.get_tvlist());

        default:
            return Py_NewRef(Py_None);
        }
    }

    Object::operator bool() const noexcept
    {
        return this->cobj != nullptr;
    }

    PyObject *Object::acquire() const
    {
        if (this->cobj)
        {
            Py_INCREF(this->cobj);
        }
        return this->cobj;
    }

    PyObject *Object::borrow() const
    {
        return this->cobj;
    }

    std::string Object::name() const
    {
        if (this->cobj)
        {
            return Object(PyType_GetName(Py_TYPE(this->cobj))).as_string().value();
        }
        else
        {
            return "";
        }
    }

    types::ValueType Object::value_type() const
    {
        if (this->cobj)
        {
            try
            {
                return Object::type_map.at(Py_TYPE(this->cobj));
            }
            catch (const std::out_of_range &)
            {
            }
        }
        return types::ValueType::NONE;
    }

    types::Value Object::as_value() const
    {
        switch (this->value_type())
        {
        case types::ValueType::BOOL:
            return this->as_bool();

        case types::ValueType::SINT:
            if (auto uint = this->as_uint())
            {
                return uint;
            }
            else
            {
                return this->as_sint();
            }

        case types::ValueType::UINT:
            return this->as_uint();

        case types::ValueType::REAL:
            return this->as_real();

        case types::ValueType::COMPLEX:
            return this->as_complex();

        case types::ValueType::STRING:
            return this->as_string();

        case types::ValueType::BYTEVECTOR:
            return this->as_bytevector();

        case types::ValueType::VALUELIST:
            if (auto tvlist = this->as_tvlist())
            {
                return tvlist.value();
            }
            else
            {
                return this->as_valuelist();
            }

        case types::ValueType::KVMAP:
            return this->as_kvmap();

        default:
            return {};
        }
    }

    std::optional<bool> Object::as_bool() const
    {
        if (this->cobj && PyBool_Check(this->cobj))
        {
            return Py_IsTrue(this->cobj);
        }
        else
        {
            return {};
        }
    }

    std::optional<types::largest_uint> Object::as_uint() const
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

    std::optional<types::largest_sint> Object::as_sint() const
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

    std::optional<double> Object::as_real() const
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

    std::optional<types::complex> Object::as_complex() const
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

    std::optional<std::string> Object::as_string() const
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

    std::optional<types::ByteVector> Object::as_bytevector() const
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

    std::optional<types::ValueList> Object::as_valuelist() const
    {
        if (this->cobj && PyList_Check(this->cobj))
        {
            types::ValueList values;
            Py_ssize_t size = PyList_Size(this->cobj);
            values.reserve(size);

            for (int c = 0; c < size; c++)
            {
                values.push_back(Object(PyList_GetItem(this->cobj, c), true).as_value());
            }
            return values;
        }
        else
        {
            return {};
        }
    }

    std::optional<types::TaggedValueList> Object::as_tvlist() const
    {
        if (this->cobj && PyList_Check(this->cobj))
        {
            types::TaggedValueList tvlist;
            Py_ssize_t size = PyList_Size(this->cobj);
            tvlist.reserve(size);

            for (int c = 0; c < size; c++)
            {
                PyObject *item = PyList_GetItem(this->cobj, c);
                if (PyTuple_Check(item) && (PyTuple_Size(item) == 2))
                {
                    types::Tag tag;
                    PyObject *tag_obj = PyTuple_GetItem(item, 0);
                    if (PyUnicode_Check(tag_obj))
                    {
                        tag = Object(tag_obj, true).as_string();
                    }
                    else if (!Py_IsNone(tag_obj))
                    {
                        return {};
                    }

                    PyObject *value_obj = PyTuple_GetItem(item, 1);
                    types::Value value = Object(value_obj, true).as_value();

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

    std::optional<types::KeyValueMap> Object::as_kvmap() const
    {
        if (this->cobj && PyDict_Check(this->cobj))
        {
            types::KeyValueMap kvmap;
            PyObject *key_obj, *value_obj;
            Py_ssize_t pos = 0;
            while (PyDict_Next(this->cobj, &pos, &key_obj, &value_obj))
            {
                if (auto key = Object(key_obj, true).as_string())
                {
                    types::Value value = Object(value_obj, true).as_value();
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

    std::unordered_map<PyTypeObject *, types::ValueType> Object::type_map = {
        {&PyBool_Type, types::ValueType::BOOL},
        {&PyUnicode_Type, types::ValueType::STRING},
        {&PyLong_Type, types::ValueType::SINT},
        {&PyFloat_Type, types::ValueType::REAL},
        {&PyComplex_Type, types::ValueType::COMPLEX},
        {&PyBytes_Type, types::ValueType::BYTEVECTOR},
        {&PyByteArray_Type, types::ValueType::BYTEVECTOR},
        {&PyList_Type, types::ValueType::VALUELIST},
        {&PyTuple_Type, types::ValueType::VALUELIST},
        {&PyDict_Type, types::ValueType::KVMAP},
    };
}  // namespace cc::python
