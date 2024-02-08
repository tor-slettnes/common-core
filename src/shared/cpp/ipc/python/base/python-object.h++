/// -*- c++ -*-
//==============================================================================
/// @file python-object.h++
/// @brief Generic data access for Python objects
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
    public:
        using Vector = std::vector<Object>;
        using Map = std::map<std::string, Object>;

    public:
        /// @brief
        ///     RAII constructor for C PyObject* instances.
        /// @param[in] borrowed
        ///     The object reference is borrowed, increment reference count
        Object(PyObject *cobj, bool borrowed = false);

        /// @brief
        ///     Copy constructor. Reference count is incremented.
        Object(const Object &other);

        /// @brief
        ///     Destructor. Reference count is decemented, which potentially
        ///     destroys the underlying object.
        virtual ~Object();

        static PyObject *pystring_from_string(const std::string &string);
        static PyObject *pybytes_from_bytes(const types::ByteVector &bytes);
        static PyObject *pytuple_from_values(const types::ValueList &values);
        static PyObject *pylist_from_values(const types::ValueList &values);
        static PyObject *pylist_from_tagged_values(const types::TaggedValueList &tvlist);
        static PyObject *pydict_from_kvmap(const types::KeyValueMap &kvmap);
        static PyObject *pyobj_from_value(const types::Value &value);

        operator bool() const noexcept;

        /// @brief
        ///     Obtain a new reference to the underlying C object
        PyObject *acquire() const;

        /// @brief
        ///     Borrow the reference to the underlying C object
        PyObject *borrow() const;

        std::string name() const;

        /// @brief
        ///    Determine the variant value type corresponding to this Python object.
        /// @return
        ///    A `types::ValueType` enum if this Python object can be represented
        ///    as a `types::Value` instance, `types::ValueType::NONE` otherwise.
        types::ValueType value_type() const;

        /// @brief
        ///    Convert this Python object to a `types::Value` instance.
        /// @return
        ///    A new `types::Value()` instance, which may be empty if this object
        ///    cannot be represented as such.
        types::Value as_value() const;

        /// @brief
        ///    Get the boolean value of a Python `bool` object.
        /// @return
        ///    The value of this object if it is a Python `bool` instance.
        std::optional<bool> as_bool() const;

        /// @brief
        ///    Get the unsigned integer value of a non-negative Python `int` instance.
        /// @return
        ///    The value of this object if it is an non-negative integer.
        std::optional<std::uint64_t> as_uint() const;

        /// @brief
        ///    Get the signed integer value of a Python `int` object.
        /// @return
        ///    The value of this object if it is a non-overflowing Python `int` instance
        std::optional<std::int64_t> as_sint() const;

        /// @brief
        ///    Get the value of a Python `float` object.
        /// @return
        ///    The value of this object if it is a Python `float` instance
        std::optional<double> as_real() const;

        /// @brief
        ///    Get the value of a Python `complex` object.
        /// @return
        ///    The value of this object if it is a Python `complex` instance
        std::optional<types::complex> as_complex() const;

        /// @brief
        ///    Get the string value of a Python `string` object.
        /// @return
        ///    The UTF-8 representation of this object if it is a Python `string` instance.
        std::optional<std::string> as_string() const;

        /// @brief
        ///    Get the value of a Python `bytes` or `bytearray` object.
        /// @return
        ///    The value of this Python object if it is a Python `bytes` or `bytearray` instance
        std::optional<types::ByteVector> as_bytevector() const;

        /// @brief
        ///    Construct a `types::ValueList` instance from this.
        /// @return
        ///    A new `types::ValueList` instance if this is a Python `list` object.
        /// @note
        ///    List items that cannot be represented as variant values are empty.
        std::optional<types::ValueList> as_valuelist() const;

        /// @brief
        ///    Attempt to construct a `types::TaggedValueList` from this.
        /// @return
        ///    A new `types::TaggedValueList()` instance if conditions are met.
        ///
        /// This object must satisfy the following conditions.
        ///  - It must be a Python `list` object
        ///  - Each list item must be a `(tag, value)` pair
        ///  - Each `tag` needs to be `None` or a string
        std::optional<types::TaggedValueList> as_tvlist() const;

        /// @brief
        ///    Construct a `types::KeyValueMap` instance from this.
        /// @return
        ///    A new `types::KeyValueMap` instance if this is a Python `dict` object.
        /// @note
        ///    List items that cannot be represented as variant values are empty.
        std::optional<types::KeyValueMap> as_kvmap() const;

    private:
        static std::unordered_map<PyTypeObject *, types::ValueType> type_map;

    protected:
        PyObject *cobj;
    };
}  // namespace cc::python
