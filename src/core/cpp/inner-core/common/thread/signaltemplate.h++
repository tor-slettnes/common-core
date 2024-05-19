/// -*- c++ -*-
//==============================================================================
/// @file signaltemplate.h++
/// @brief Adaptation of Signal/Slot pattern - inline functions
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "string/format.h++"

#include <string>
#include <functional>
#include <optional>
#include <ostream>
#include <unordered_map>
#include <map>
#include <mutex>

namespace core::signal
{
    //==========================================================================
    // Types

    using Handle = std::string;

    // Synchronized to `cc.protobuf.signal.MappingAction` in `signal.proto`.
    // We do not use that here, since ProtoBuf support is optional, not part of
    // the "inner core".

    enum MappingAction
    {
        MAP_NONE,
        MAP_ADDITION,
        MAP_REMOVAL,
        MAP_UPDATE,
    };

    //==========================================================================
    /// @class BaseSignal
    /// @brief Abstract/untyped base for Signal<T> and MappingSignal<T>

    class BaseSignal
    {
    public:
        /// @brief Register a signal handler for signals of the provided template type.
        /// @param[in] name
        ///     Identity of this signal, for logging purposes.
        /// @param[in] caching
        ///     Store last emitted value and replay to new receivers.

        BaseSignal(const std::string &name,
                   bool caching = false);

        void set_caching(bool caching);
        std::string name() const;

    protected:
        Handle unique_handle() const;
        void safe_invoke(const std::string &receiver,
                         const std::function<void()> &f);

    protected:
        std::recursive_mutex mtx_;
        std::string name_;
        bool caching_;
    };

    //==========================================================================
    /// @class VoidSignal
    /// @brief Event notification without data

    class VoidSignal : public BaseSignal
    {
        using Super = BaseSignal;

    public:
        using Slot = std::function<void()>;

        VoidSignal(const std::string &id);

        /// @brief Register a signal handler for signals of the provided template type.
        /// @param[in] slot
        ///     A callback function, invoked whenever the signal is emitted
        /// @return
        ///     A unique handle, which can later be used to disconnect
        Handle connect(const Slot &slot);

        /// @brief Register a signal handler for signals of the provided template type.
        /// @param[in] handle
        ///     Unique identity of the callback handler, to be used for
        ///     subsequent cancellation.
        /// @param[in] slot
        ///     A callback function, invoked whenever the signal is emitted
        void connect(const Handle &handle, const Slot &slot);

        /// @brief
        ///     Unregister a handler for signals of the provided template type.
        /// @param[in] handle
        ///     Identity of the handler to be removed.
        void disconnect(const Handle &handle);

        /// @brief
        ///     Emit a signal to registered receivers
        /// @return
        ///     The number of connected slots to which the signal was emitted
        size_t emit();

        /// @brief
        ///    Obtain number of current connections.
        /// @return
        ///    Number of connected slots
        size_t connection_count();

    protected:
        void callback(const std::string &receiver, const Slot &method);

    private:
        std::unordered_map<std::string, Slot> slots_;
    };

    //==========================================================================
    /// @class DataSignal
    /// @brief Template for emitting arbitrary data types as signals,
    ///    and to register receivers that will be notified on change.
    ///
    /// Example:
    /// @code
    ///      void on_my_signal(const MyDataType &signal_data) {...}
    ///      ...
    ///      DataSignal<MyDataType> my_signal;
    ///      signal.connect(on_my_signal);
    ///      ...
    ///      MyDataType mydata = {...};
    ///      my_signal.emit(mydata);
    /// @endcode

    template <class DataType>
    class DataSignal : public BaseSignal
    {
        using Super = BaseSignal;

    public:
        using Slot = std::function<void(const DataType &)>;

        DataSignal(const std::string &id, bool caching = false);

        /// @brief Register a signal handler for signals of the provided template type.
        /// @param[in] slot
        ///     A callback function, invoked whenever the signal is emitted
        /// @return
        ///     A unique handle, which can later be used to disconnect
        Handle connect(const Slot &slot);

        /// @brief Register a signal handler for signals of the provided template type.
        /// @param[in] handle
        ///     Unique identity of the callback handler, to be used for
        ///     subsequent cancellation.
        /// @param[in] slot
        ///     A callback function, invoked whenever the signal is emitted
        void connect(const Handle &handle, const Slot &slot);

        /// @brief
        ///     Unregister a handler for signals of the provided template type.
        /// @param[in] handle
        ///     Identity of the handler to be removed.
        void disconnect(const Handle &handle);

        /// @brief
        ///     Emit a signal to registered receivers of the provided data type.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted
        size_t emit(const DataType &value);

        /// @brief
        ///     Emit signal only if the current value differs from the previous one.
        /// @note
        ///     This only works if the `cache` option is used.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted
        size_t emit_if_changed(const DataType &value);

        /// @brief
        ///    Get the current cached value, if any.
        /// @return
        ///    std::optional<DataType> object
        std::optional<DataType> get_cached();

        /// @brief
        ///    Obtain number of current connections.
        /// @return
        ///    Number of connected slots
        size_t connection_count();

    protected:
        virtual void emit_cached_to(const std::string &handle,
                                    const Slot &slot);

        size_t sendall(const DataType &value);

        void update_cache(const DataType &value);

        void callback(const std::string &receiver,
                             const Slot &method,
                             const DataType &value);

    private:
        std::optional<DataType> cached_;
        std::unordered_map<std::string, Slot> slots_;
    };


    //==========================================================================
    /// @class MappingSignal
    /// @brief Abstract base for signals that emit mappable data alongside
    ///   with their mapping key and a change type (added, removed, updated).
    ///
    ///
    /// Example:
    /// @code
    ///      void on_my_signal(MappingSignal::MappingAction change,
    ///                       const std::string &key,
    ///                       const MyDataType &data) {...}
    ///      ...
    ///      MappingSignal<MyDataType> my_mapping_signal;
    ///      signal.connect(on_my_mapping_signal);
    ///      ...
    ///      MyDataType mydata = {...};
    ///      my_mapping_signal.emit(MAP_ADDITION, "key", mydata);
    ///      my_mapping_signal.emit(MAP_UPDATE, "key", mydata);
    ///      my_mapping_signal.clear("key");
    /// @endcode


    template <class DataType, class KeyType = std::string>
    class MappingSignal : public BaseSignal
    {
        using Super = BaseSignal;

    public:
        using Slot = std::function<void(MappingAction, const KeyType &, const DataType &)>;

        MappingSignal(const std::string &id, bool caching = false);

        /// @brief Register a signal handler for this signal.
        /// @param[in] slot
        ///     A function invoked whenever signal data is emitted
        /// @return
        ///     Unique handle which can later be used to disconnect
        Handle connect(const Slot &slot);

        /// @brief Register a signal handler for this signal.
        /// @param[in] handle
        ///     Unique handle which can later be used to disconnect
        /// @param[in] slot
        ///     A function invoked whenever signal data is emitted
        void connect(const Handle &handle, const Slot &slot);

        /// @brief
        ///     Unregister a callback handler for signals of the provided template type.
        /// @param[in] handle
        ///     Identity identity of the callback handler to be removed.

        void disconnect(const Handle &handle);

        /// @brief
        ///     Emit a signal to registered receivers of the provided data type.
        /// @param[in] change
        ///     What change is being emitted: ADDITION, REMOVAL, UPDATE.
        /// @param[in] key
        ///     Mapping key
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted
        size_t emit(MappingAction change, const KeyType &key, const DataType &value);

        /// @brief
        ///     Emit a signal to registered receivers of the provided data type.
        /// @param[in] key
        ///     Mapping key
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted
        size_t emit(const KeyType &key, const DataType &value);

        /// @brief
        ///     Emit signal only if the current value differs from the previous one.
        /// @note
        ///     This only works if the `cache` option is used.
        /// @param[in] key
        ///     Mapping key
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted
        size_t emit_if_changed(const KeyType &key, const DataType &value);

        /// @brief
        ///     Emit a REMOVED signal.
        /// @param[in] key
        ///     Mapping key.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted
        size_t clear(const KeyType &key, const DataType &value = {});

        /// @brief
        ///     Emit a REMOVED signal if \p key is still in in the signal cache
        /// @param[in] key
        ///     Mapping key.
        size_t clear_if_cached(const KeyType &key);

        /// @brief
        ///    Get the current cached value, if any.
        /// @return
        ///    std::optional<DataType> object
        std::unordered_map<KeyType, DataType> get_cached();

        /// @brief
        ///    Get the most recent data value emitted for a given key
        /// @param[in] key
        ///    Mapping key
        /// @return
        ///    Most recent data value emitted for the specified key, if any
        std::optional<DataType> get_cached(const std::string &key);

        /// @brief
        ///    Get the most recent data value emitted for a given key
        /// @param[in] key
        ///    Mapping key
        /// @param[in] fallback
        ///    Data value to return if the specified key doesn't exist in the cache
        /// @return
        ///    Most recent data value emitted for the specified key, otherwise
        ///    the provided fallback value.
        DataType get_cached(const std::string &key, const DataType &fallback);

        /// @brief
        ///    Indicate whether the specified mapping currently exists in the cache
        /// @param[in] key
        ///    Mapping key
        /// @return
        ///    Boolean indicator of whether the specified key exists
        bool is_cached(const std::string &key) noexcept;

        /// @brief
        ///    Obtain number of current connections.
        /// @return
        ///    Number of connected slots
        size_t connection_count();

        /// @brief
        ///     Update cache, emit deltas as addition/update/removal signals
        /// @param[in] update
        ///     Updated map
        /// @return
        ///     The number of signals emitted
        template <class MapType>
        size_t synchronize(const MapType &update);

    protected:
        void update_cache(const KeyType &key, const DataType &value);

        void emit_cached_to(const std::string &handle,
                                   const Slot &callback);

        size_t sendall(MappingAction change,
                              const KeyType &key,
                              const DataType &value = {});

        void callback(const std::string &receiver,
                             const Slot &method,
                             MappingAction change,
                             const KeyType &key,
                             const DataType &value);

    private:
        std::unordered_map<KeyType, DataType> cached_;
        std::unordered_map<std::string, Slot> slots_;
    };


    //==========================================================================
    // I/O stream support

    std::ostream &operator<<(std::ostream &stream, MappingAction change);
}  // namespace core::signal

#include "signaltemplate.i++"
