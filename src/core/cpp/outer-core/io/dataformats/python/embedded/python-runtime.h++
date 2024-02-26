// -*- c++ -*-
//==============================================================================
/// @file python-runtime.h++
/// @brief Python runtime environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "python-builtin.h++"
#include "python-exception.h++"
#include "types/filesystem.h++"
#include "types/value.h++"

namespace core::python
{
    /// @brief Abstract provider for process invocation
    class Runtime
    {
        using This = Runtime;

    public:
        static void global_init();
        static void global_cleanup();

        ContainerObject &import(const std::string &module_name);

        /// @brief
        ///    Call an imported Python method with variant inputs and return value
        /// @param[in] module_name
        ///    Module name in which to find method.  If empty, use the global scope.
        /// @param[in] method_name
        ///    Method name to be called
        /// @param[in] args
        ///    Positional arguments
        /// @param[in] args
        ///    Named arguments
        /// @return
        ///    Returned value from the called method, decoded as a variant Value
        types::Value call(const std::optional<std::string> &module_name,
                          const std::string &method_name,
                          const types::ValueList &args = {},
                          const types::KeyValueMap &kwargs = {});

        /// @brief
        ///    Call an imported Python method with encoded Python inputs and return value
        /// @param[in] module_name
        ///    Module name in which to find method.  If empty, use the global scope.
        /// @param[in] method_name
        ///    Method name to be called
        /// @param[in] args
        ///    A managed object containing a tuple with positional input arguments
        /// @param[in] args
        ///    A managed object containing a dictionary with keyword (named) input arguments
        /// @return
        ///    Returned value from the called method
        ContainerObject call(const std::optional<std::string> &module_name,
                             const std::string &method_name,
                             const SimpleObject::Vector &args,
                             const SimpleObject::Map &kwargs = {});

        /// @brief
        ///    Call an imported Python method with encoded Python inputs and return value
        /// @param[in] module_name
        ///    Module name in which to find method.  If empty, use the global scope.
        /// @param[in] method_name
        ///    Method name to be called
        /// @param[in] args_tuple
        ///    A managed object containing a tuple with positional input arguments
        /// @param[in] kwargs_map
        ///    A managed object containing a dictionary with keyword (named) input arguments
        /// @return
        ///    Return value from the called method as a Python object
        ContainerObject call(const std::optional<std::string> &module_name,
                             const std::string &method_name,
                             const SimpleObject &args_tuple,
                             const SimpleObject &kwargs_dict);

    private:
        ContainerObject &get_container(
            const std::optional<std::string> &module_name);

        Exception get_exception() const;

    private:
        Builtin builtin;
        std::unordered_map<std::string, ContainerObject> modules;
    };

    extern std::unique_ptr<Runtime> runtime;
}  // namespace core::python
