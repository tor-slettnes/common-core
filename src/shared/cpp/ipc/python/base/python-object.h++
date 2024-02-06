/// -*- c++ -*-
//==============================================================================
/// @file python-object.h++
/// @brief Serialization/deserializaiton of Python objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "types/value.h++"

namespace cc::python
{
    class Object
    {
    protected:
        Object(PyObject *cobj, bool borrowed=false);
        virtual ~Object();

    public:
        static Object create_from_value(const types::Value &value);
        static Object create_from_pickle(const types::ByteVector &bytes);

        types::ValueType value_type() const;

        types::Value as_value() const;
        bool as_bool() const;
        ulong as_uint() const;
        long as_sint() const;
        double as_real() const;
        types::complex as_complex() const;
        std::string as_string() const;
        types::ByteVector as_bytevector() const;
        types::ValueList as_valuelist() const;
        types::TaggedValueList as_tvlist() const;
        types::KeyValueMap as_kvmap() const;
        types::ByteVector as_pickle() const;

    private:
        static PyObject *pyobj_from_value(const types::Value &value);

    private:
        static std::unordered_map<PyTypeObject*, types::ValueType> type_map;
        PyObject *cobj;
    };
}  // namespace cc::python
