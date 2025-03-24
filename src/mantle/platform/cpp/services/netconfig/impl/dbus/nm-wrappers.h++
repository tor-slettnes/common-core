/// -*- c++ -*-
//==============================================================================
/// @file nm-wrappers.h++
/// @brief Network Manager object wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "netconfig.h++"
#include "dbus-proxies.h++"
#include "types/value.h++"
#include "status/exceptions.h++"

// Third party
#include <glibmm/refptr.h>
#include <giomm/dbusconnection.h>
#include <giomm/dbusproxy.h>
#include <nm-dbus-interface.h>

// C++ STL
#include <unordered_map>
#include <set>

namespace netconfig::dbus
{
    //==========================================================================
    // NetworkManager proxy container.instance

    extern core::dbus::ProxyContainer container;

    //==========================================================================
    /// @class DataWrapper<T>
    /// @brief DBus Proxy wrapper with abstraction for data storage

    template <class DataType>
    class DataWrapper : public core::dbus::ProxyWrapper, public DataType
    {
    public:
        using core::dbus::ProxyWrapper::ProxyWrapper;
    };

    //==========================================================================
    /// @class MappedDataWrapper<T>
    /// @brief DBus Proxy wrapper with abstraction for mapped data storage

    template <class MappedDataType>
    class MappedDataWrapper : public DataWrapper<MappedDataType>
    {
    public:
        using DataWrapper<MappedDataType>::DataWrapper;
        std::string identifier() const override
        {
            return core::str::format("%s.%s(%r, %r)",
                                     core::str::stem(this->servicename, "."),
                                     core::str::stem(this->interfacename, "."),
                                     this->shortpath(),
                                     this->key());
        }
    };

    //==========================================================================
    /// @brief Search for a specific NetConfig proxy object by key
    /// @param[in] key
    ///    Lookup key, as designated in data types in netconfig-structs.h
    /// @param[in] required
    ///    Raise an exception if true and the object does not exist
    /// @return
    ///    Reference to object proxy for the specified type/key.

    template <class WrapperType>
    inline std::shared_ptr<WrapperType> lookup(const Key &key, bool required)
    {
        for (const auto &[path, obj] : dbus::container.instances<WrapperType>())
        {
            if (obj->key() == key)
            {
                return obj;
            }
        }

        if (required)
        {
            throwf_args(core::exception::NotFound,
                        ("Unknown %s: %s", TYPE_NAME_BASE(WrapperType), key),
                        key);
        }

        return {};
    }

    //==========================================================================
    /// @brief Return a map of all available data structures of the specified type
    /// @param
    ///    Reference to data objects for the specified type

    template <class WrapperType, class DataType>
    inline std::unordered_map<Key, std::shared_ptr<DataType>> datamap()
    {
        std::unordered_map<Key, std::shared_ptr<DataType>> map;
        for (const auto &[path, ref] : dbus::container.instances<WrapperType>())
        {
            map.emplace(ref->key(), ref);
        }
        return map;
    }

} // namespace netconfig::dbus
