// -*- c++ -*-
//==============================================================================
/// @file python-runtime.c++
/// @brief Python runtime environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "python-runtime.h++"
#include "python-exception.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"
#include "platform/symbols.h++"
#include "platform/init.h++"

namespace cc::python
{
    void Runtime::global_init()
    {
        Py_Initialize();
        runtime = std::make_unique<Runtime>();
    }

    void Runtime::global_cleanup()
    {
        runtime.reset();
        Py_FinalizeEx();
    }

    ContainerObject &Runtime::import(const std::string &module_name)
    {
        try
        {
            return this->modules.at(module_name);
        }
        catch (const std::out_of_range &)
        {
            if (SimpleObject py_module_name = PyUnicode_DecodeFSDefaultAndSize(
                    module_name.data(),
                    module_name.size()))
            {
                if (PyObject *py_module = PyImport_Import(py_module_name.borrow()))
                {
                    auto [it, inserted] = this->modules.try_emplace(module_name, py_module);
                    return it->second;
                }
            }

            throw this->get_exception();
        }
    }

    types::Value Runtime::call(const std::optional<std::string> &module_name,
                               const std::string &method_name,
                               const types::ValueList &args,
                               const types::KeyValueMap &kwargs)
    {
        SimpleObject py_args(SimpleObject::pytuple_from_values(args));
        SimpleObject py_kwargs(SimpleObject::pydict_from_kvmap(kwargs));
        return this->call(module_name, method_name, py_args, py_kwargs).as_value();
    }

    ContainerObject Runtime::call(const std::optional<std::string> &module_name,
                                  const std::string &method_name,
                                  const SimpleObject::Vector &args,
                                  const SimpleObject::Map &kwargs)
    {
        // Construct a Python object for the positional arguments
        SimpleObject py_args(PyTuple_New(args.size()));
        for (uint c = 0; c < args.size(); c++)
        {
            PyTuple_SET_ITEM(py_args.borrow(), c, args.at(c).acquire());
        }

        // Construct a Python object for the key/value arguments
        SimpleObject py_kwargs(PyDict_New());
        for (auto &[key, obj] : kwargs)
        {
            SimpleObject key_obj(PyUnicode_DecodeUTF8(key.data(), key.size(), nullptr));
            PyDict_SetItem(py_kwargs.borrow(), key_obj.borrow(), obj.borrow());
        }

        return this->call(module_name, method_name, py_args, py_kwargs);
    }

    ContainerObject Runtime::call(const std::optional<std::string> &module_name,
                                  const std::string &method_name,
                                  const SimpleObject &args,
                                  const SimpleObject &kwargs)
    {
        ContainerObject &container = this->get_container(module_name);
        if (auto method = container.find_qualified_symbol(method_name))
        {
            if (PyCallable_Check(method.borrow()))
            {
                if (PyObject *result = PyObject_Call(method.borrow(),
                                                     args.borrow(),
                                                     kwargs.borrow()))
                {
                    return result;
                }
                else
                {
                    throw this->get_exception();
                }
            }
            else
            {
                throw exception::FailedPrecondition(
                    "Python object is not callable",
                    {
                        {"symbol", method.name()},
                        {"type", method.type_name()},
                    });
            }
        }
        else
        {
            throwf_args(exception::NotFound,
                        ("Symbol not found: %s", method_name),
                        method_name);
        }
    }

    ContainerObject &Runtime::get_container(
        const std::optional<std::string> &module_name)
    {
        return module_name ? this->import(module_name.value()) : this->builtin;
    }

    Exception Runtime::get_exception() const
    {
#if PY_VERSION_HEX >= 0x030C0000  // Python >= 3.12  has a new exception API
        ContainerObject exc(PyError_GetRaisedException());
        return Exception(exc.to_string(), exc.type_name(), exc.attributes_as_kvmap());
#else
        PyObject *py_type, *py_value, *py_tb;
        PyErr_Fetch(&py_type, &py_value, &py_tb);

        ContainerObject type(py_type);
        ContainerObject text(py_value);

        Py_XDECREF(py_tb);
        return Exception(text.to_string(), type.name(), {});
#endif
    }

    std::unique_ptr<Runtime> runtime;

    static cc::platform::InitTask py_init("Python init", Runtime::global_init);
    static cc::platform::ExitTask py_cleanup("Python cleanup", Runtime::global_cleanup);


}  // namespace cc::python
