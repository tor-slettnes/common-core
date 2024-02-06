/// -*- c++ -*-
//==============================================================================
/// @file python-object.c++
/// @brief Serialization/deserializaiton of Python objects
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
            Py_INCREF(this->cobj);
        }
    }

    Object::~Object()
    {
        if (this->cobj)
        {
            Py_DECREF(this->cobj);
        }
    }

    Object Object::create_from_value(const types::Value &value)
    {
        return Object::pyobj_from_value(value);
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
            if (auto *s = value.get_if<std::string>())
            {
                return PyUnicode_DecodeUTF8(s->data(), s->size(), nullptr);
            }

        case types::ValueType::BYTEVECTOR:
            if (const auto *bytes = value.get_if<types::ByteVector>())
            {
                return PyBytes_FromStringAndSize(
                    reinterpret_cast<const char *>(bytes->data()),
                    bytes->size());
            }
            break;

        case types::ValueType::VALUELIST:
            if (auto values = value.get_valuelist())
            {
                PyObject *list = PyList_New(values->size());
                for (uint c = 0; c < values->size(); c++)
                {
                    PyList_SET_ITEM(list, c, Object::pyobj_from_value(values->at(c)));
                }
                return list;
            }
            break;

        case types::ValueType::KVMAP:
            if (auto kvmap = value.get_kvmap())
            {
                PyObject *dict = PyDict_New();
                for (const auto &[key, value] : *kvmap)
                {
                    PyObject *key_obj = PyUnicode_DecodeUTF8(key.data(), key.size(), nullptr);
                    PyObject *value_obj = Object::pyobj_from_value(value);
                    PyDict_SetItem(dict, key_obj, value_obj);
                    Py_DECREF(key_obj);
                    Py_DECREF(value_obj);
                }
                return dict;
            }
            break;

        case types::ValueType::TVLIST:
            if (auto tvlist = value.get_tvlist())
            {
                PyObject *list = PyList_New(tvlist->size());
                for (uint c = 0; c < tvlist->size(); c++)
                {
                    const auto &[tag, value] = tvlist->at(c);
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
            break;

        default:
            break;
        }

        return Py_NewRef(Py_None);
    }

    Object Object::create_from_pickle(const types::ByteVector &bytes)
    {
        return Py_NewRef(Py_None);
    }

    types::ValueType Object::value_type() const
    {
        try
        {
            return Object::type_map.at(Py_TYPE(this->cobj));
        }
        catch (const std::out_of_range &)
        {
            return types::ValueType::NONE;
        }
    }

    types::Value Object::as_value() const
    {
        switch (this->value_type())
        {
        case types::ValueType::BOOL:
            return this->as_bool();

        case types::ValueType::SINT:
            try
            {
                return this->as_sint();
            }
            catch (const std::overflow_error &e)
            {
                return this->as_uint();
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
            try
            {
                return this->as_tvlist();
            }
            catch (const std::invalid_argument &e)
            {
                return this->as_valuelist();
            }

        case types::ValueType::KVMAP:
            return this->as_kvmap();

        default:
            return {};
        }
    }

    bool Object::as_bool() const
    {
        if (PyBool_Check(this->cobj))
        {
            return Py_IsTrue(this->cobj);
        }
        else
        {
            return false;
        }
    }

    types::largest_uint Object::as_uint() const
    {
        if (PyLong_Check(this->cobj))
        {
            types::largest_uint value = PyLong_AsUnsignedLongLong(this->cobj);
            if (PyErr_Occurred())
            {
                throw std::runtime_error("Python long to unsigned conversion error");
            }
            return value;
        }
        else
        {
            return 0;
        }
    }

    types::largest_sint Object::as_sint() const
    {
        if (PyLong_Check(this->cobj))
        {
            int overflow = 0;
            types::largest_sint value = PyLong_AsLongLongAndOverflow(this->cobj, &overflow);
            if (PyErr_Occurred())
            {
                if (overflow)
                {
                    throw std::overflow_error("Python long to signed conversion overflow");
                }
                else
                {
                    throw std::runtime_error("Python long to signed conversion error");
                }
            }
            return value;
        }
        else
        {
            return 0;
        }
    }

    double Object::as_real() const
    {
        if (PyFloat_Check(this->cobj))
        {
            return PyFloat_AsDouble(this->cobj);
        }
        else
        {
            return 0.0;
        }
    }

    types::complex Object::as_complex() const
    {
        if (PyComplex_Check(this->cobj))
        {
            return {PyComplex_RealAsDouble(this->cobj),
                    PyComplex_ImagAsDouble(this->cobj)};
        }
        else
        {
            return {0.0, 0.0};
        }
    }

    std::string Object::as_string() const
    {
        if (PyUnicode_Check(this->cobj))
        {
            if (Py_ssize_t size = 0;
                const char *data = PyUnicode_AsUTF8AndSize(this->cobj, &size))
            {
                return std::string(data, size);
            }
        }
        return {};
    }

    types::ValueList Object::as_valuelist() const
    {
        types::ValueList values;
        if (PyList_Check(this->cobj))
        {
            Py_ssize_t size = PyList_Size(this->cobj);
            values.reserve(size);
            for (int c = 0; c < size; c++)
            {
                values.push_back(Object(PyList_GetItem(this->cobj, c), true).as_value());
            }
        }
        return values;
    }

    types::TaggedValueList Object::as_tvlist() const
    {
        types::TaggedValueList tvlist;

        if (PyList_Check(this->cobj))
        {
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
                        throw std::invalid_argument("Not a tag/value list");
                    }

                    PyObject *value_obj = PyTuple_GetItem(item, 1);
                    types::Value value = Object(value_obj).as_value();

                    tvlist.emplace_back(tag, value);
                }
                else
                {
                    throw std::invalid_argument("Not a tag/value list");
                }
            }
        }
        return tvlist;
    }

    types::KeyValueMap Object::as_kvmap() const
    {
        types::KeyValueMap kvmap;
        if (PyDict_Check(this->cobj))
        {
            PyObject *key_obj, *value_obj;
            Py_ssize_t pos = 0;
            while (PyDict_Next(this->cobj, &pos, &key_obj, &value_obj))
            {
                std::string key = Object(key_obj, true).as_string();
                types::Value value = Object(value_obj, true).as_value();
                kvmap.insert_or_assign(key, value);
            }
        }
        return kvmap;
    }

    types::ByteVector Object::as_pickle() const
    {
        return {};
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
