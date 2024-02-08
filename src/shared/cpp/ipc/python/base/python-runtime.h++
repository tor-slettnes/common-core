// -*- c++ -*-
//==============================================================================
/// @file python-runtime.h++
/// @brief Python runtime environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "python-object.h++"

#include "types/filesystem.h++"
#include "types/value.h++"

namespace cc::python
{
    /// @brief Abstract provider for process invocation
    class Runtime
    {
        using This = Runtime;

    public:
        Runtime();
        Runtime(const std::string &module_name);

        static void initialize();
        static void finalize(int signal = 0);

        void import(const std::string &module_name);

        /// @brief
        ///    Call a named Python method with variant inputs and return value
        /// @param[in] method
        ///    Method name to be called
        /// @param[in] args
        ///    Positional arguments
        /// @param[in] args
        ///    Named arguments
        /// @return
        ///    Returned value from the called method, decoded as a variant Value
        types::Value call(const std::string &method,
                          const types::ValueList &args = {},
                          const types::KeyValueMap &kwargs = {});

        /// @brief
        ///    Call a named Python method with Python objects as inputs and returned value.
        /// @param[in] method
        ///    Method name to be called
        /// @param[in] args
        ///    Positional arguments
        /// @param[in] args
        ///    Named arguments
        /// @return
        ///    Returned value from the called method
        Object call(const std::string &method,
                    const Object::Vector &args = {},
                    const Object::Map &kwargs = {});

        /// @brief
        ///    Call a Python method with encoded Python inputs and return value
        /// @param[in] method_name
        ///    A Python object representing the method name to be called
        /// @param[in] args_tuple
        ///    A Python tuple with positional input arguments
        /// @param[in] kwargs_map
        ///    A Python dictionary with keyword (named) input arguments
        /// @return
        ///    Return value from the called method as a Python object
        Object call(const Object &method_name,
                    const Object &args_tuple,
                    const Object &kwargs_dict);

    private:
        static bool initialized;
        std::shared_ptr<Object> module;
    };
}  // namespace cc::python
