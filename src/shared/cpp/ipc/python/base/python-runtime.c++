// -*- c++ -*-
//==============================================================================
/// @file python-runtime.c++
/// @brief Python runtime environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "python-runtime.h++"
#include "python-exception.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"
#include "application/init.h++"

namespace cc::python
{
    Runtime::Runtime()
    {
        this->initialize();
    }

    Runtime::Runtime(const std::string &module)
    {
        this->initialize();
        this->import(module);
    }

    void Runtime::initialize()
    {
        if (!Runtime::initialized)
        {
            Py_Initialize();
            application::signal_shutdown.connect(TYPE_NAME_BASE(Runtime), Runtime::finalize);
            Runtime::initialized = true;
        }
    }

    void Runtime::finalize(int signal)
    {
        application::signal_shutdown.disconnect(TYPE_NAME_BASE(Runtime));
        Py_FinalizeEx();
    }

    void Runtime::import(const std::string &module_name)
    {
        if (Object py_module_name = PyUnicode_DecodeFSDefaultAndSize(
                module_name.data(),
                module_name.size()))
        {
            if (Object py_module = PyImport_Import(py_module_name.borrow()))
            {
                this->module = std::make_shared<Object>(py_module);
                return;
            }
        }
        throw Exception(PyErr_Occurred(), module_name);
    }

    types::Value Runtime::call(const std::string &method,
                               const types::ValueList &args,
                               const types::KeyValueMap &kwargs)
    {
        Object py_method(Object::pystring_from_string(method));
        Object py_args(Object::pytuple_from_values(args));
        Object py_kwargs(Object::pydict_from_kvmap(kwargs));
        Object py_result(this->call(py_method, py_args, py_kwargs));
        return py_result.as_value();
    }

    Object Runtime::call(const std::string &method,
                         const Object::Vector &args,
                         const Object::Map &kwargs)
    {
        // Construct a Python object for the method name
        Object py_method(Object::pystring_from_string(method));

        // Construct a Python object for the positional arguments
        Object py_args(PyTuple_New(args.size()));
        for (uint c = 0; c < args.size(); c++)
        {
            PyTuple_SET_ITEM(py_args.borrow(), c, args.at(c).acquire());
        }

        // Construct a Python object for the key/value arguments
        Object py_kwargs(PyDict_New());
        for (auto &[key, obj] : kwargs)
        {
            Object key_obj(PyUnicode_DecodeUTF8(key.data(), key.size(), nullptr));
            PyDict_SetItem(py_kwargs.borrow(), key_obj.borrow(), obj.borrow());
        }
        return this->call(py_method, py_args, py_kwargs);
    }

    Object Runtime::call(const Object &method_name,
                         const Object &args,
                         const Object &kwargs)
    {
        if (this->module)
        {
            if (Object py_func = PyObject_GetAttr(this->module->borrow(), method_name.borrow()))
            {
                if (PyCallable_Check(py_func.borrow()))
                {
                    if (PyObject *result = PyObject_Call(py_func.borrow(),
                                                         args.borrow(),
                                                         kwargs.borrow()))
                    {
                        return result;
                    }
                    else
                    {
                        throw Exception(PyErr_Occurred(), this->module->name());
                    }
                }
                else
                {
                    throwf_args(Exception,
                                ("Python symbol is not callable: %r", method_name),
                                this->module->name());
                }
            }
            else
            {
                throwf_args(Exception,
                            ("Method name not found: %r", method_name.as_string().value()),
                            this->module->name());
            }
        }
        else
        {
            throw exception::FailedPrecondition("No Python module has been imported");
        }
        return nullptr;
    }

    bool Runtime::initialized = false;

}  // namespace cc::python
