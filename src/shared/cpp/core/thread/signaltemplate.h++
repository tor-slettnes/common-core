/// -*- c++ -*-
//==============================================================================
/// @file signaltemplate.h++
/// @brief Simple non-buffered adaptation of Signal/Slot pattern
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

namespace shared::signal
{
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
        /// @return
        ///     A reference to the std::function that was passed in.

        BaseSignal(const std::string &name,
                   bool caching = false);

        void set_caching(bool caching);
        std::string name();

    protected:
        void safe_invoke(const std::string &receiver,
                         const std::function<void()> &f);

    protected:
        std::recursive_mutex mtx_;
        std::string name_;
        bool caching_;
    };

    //==========================================================================
    /// @class VoidSignal
    /// @class Event notification without data

    class VoidSignal : public BaseSignal
    {
        using Super = BaseSignal;

    public:
        using Slot = std::function<void()>;

        VoidSignal(const std::string &id);
        void connect(const std::string &id, const Slot &slot);
        void disconnect(const std::string &id);
        size_t emit();
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

        /// @fn connect()
        /// @brief Register a signal handler for signals of the provided template type.
        /// @param[in] id
        ///     Unique identity of the callback handler, to be used for
        ///     subsequent cancellation.
        /// @param[in] slot
        ///     A callback function, invoked whenever the signal is emitted
        /// @return
        ///     A reference to the std::function that was passed in.

        void connect(const std::string &id, const Slot &slot);

        /// @fn disconnect()
        /// @brief
        ///     Unregister a handler for signals of the provided template type.
        /// @param[in] id
        ///     Identity of the handler to be removed.

        void disconnect(const std::string &id);

        /// @fn emit()
        /// @brief
        ///     Emit a signal to registered receivers of the provided data type.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted

        size_t emit(const DataType &value);

        /// @fn emit_if_changed()
        /// @brief
        ///     Emit signal only if the current value differs from the previous one.
        /// @note
        ///     This only works if the `cache` option is used.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted

        size_t emit_if_changed(const DataType &value);

        /// @fn get_cached()
        /// @brief
        ///    Get the current cached value, if any.
        /// @return
        ///    std::optional<DataType> object

        std::optional<DataType> get_cached();

        /// @fn connection_count()
        /// @brief
        ///    Obtain number of current connections.
        /// @return
        ///    Number of connected slots

        size_t connection_count();

    protected:
        virtual void emit_cached_to(const std::string &id,
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
    ///      void on_my_signal(MappingSignal::MappingChange change,
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

    enum MappingChange
    {
        MAP_NONE,
        MAP_ADDITION,
        MAP_REMOVAL,
        MAP_UPDATE,
    };

    template <class DataType, class KeyType = std::string>
    class MappingSignal : public BaseSignal
    {
        using Super = BaseSignal;

    public:
        using Slot = std::function<void(MappingChange, const KeyType &, const DataType &)>;

        MappingSignal(const std::string &id, bool caching = false);

        /// @fn connect()
        /// @brief Register a pair of signal handlers for this signal.
        /// @param[in] id
        ///     Unique identity of the callback handler, to be used for
        ///     subsequent cancellation.
        /// @param[in] slot
        ///     A function invoked whenever signal data is emitted
        void connect(const std::string &id, const Slot &slot);

        /// @fn disconnect()
        /// @brief
        ///     Unregister a callback handler for signals of the provided template type.
        /// @param[in] id
        ///     Identity identity of the callback handler to be removed.

        void disconnect(const std::string &id);

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
        size_t emit(MappingChange change, const KeyType &key, const DataType &value);

        /// @brief
        ///     Emit a signal to registered receivers of the provided data type.
        /// @param[in] key
        ///     Mapping key
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted

        size_t emit(const KeyType &key, const DataType &value);

        /// @fn emit_if_changed()
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

        /// @fn clear()
        /// @brief
        ///     Emit a REMOVED signal.
        /// @param[in] key
        ///     Mapping key.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted

        size_t clear(const KeyType &key, const DataType &value = {});

        /// @fn clear_if_cached()
        /// @brief
        ///     Emit a REMOVED signal if \p key is still in in the signal cache
        /// @param[in] key
        ///     Mapping key.
        size_t clear_if_cached(const KeyType &key);

        /// @fn get_cached()
        /// @brief
        ///    Get the current cached value, if any.
        /// @return
        ///    std::optional<DataType> object
        std::unordered_map<KeyType, DataType> get_cached();

        std::optional<DataType> get_cached(const std::string &key);

        DataType get_cached(const std::string &key, const DataType &fallback);

        bool is_cached(const std::string &key) noexcept;

        /// @fn connection_count()
        /// @brief
        ///    Obtain number of current connections.
        /// @return
        ///    Number of connected slots
        size_t connection_count();

        /// @fn synchronize()
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

        void emit_cached_to(const std::string &id,
                                   const Slot &callback);

        size_t sendall(MappingChange change,
                              const KeyType &key,
                              const DataType &value = {});

        void callback(const std::string &receiver,
                             const Slot &method,
                             MappingChange change,
                             const KeyType &key,
                             const DataType &value);

    private:
        std::unordered_map<KeyType, DataType> cached_;
        std::unordered_map<std::string, Slot> slots_;
    };


    //==========================================================================
    // I/O stream support

    std::ostream &operator<<(std::ostream &stream, MappingChange change);
}  // namespace shared::signal

#include "signaltemplate.i++"
