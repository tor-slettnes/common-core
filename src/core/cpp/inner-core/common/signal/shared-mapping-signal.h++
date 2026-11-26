/// -*- c++ -*-
//==============================================================================
/// @file mapping-signal.h++
/// @brief Signal with mappable data as payload
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "signal/base-signal.h++"
#include "types/create-shared.h++"

namespace cc::core::signal
{

    //==========================================================================
    /// @class SharedMappingSignal
    /// @brief
    ///   Template for signals that emit mappable shared_ptr references
    ///   with their mapping key and a change type (added, removed, updated).
    ///
    ///
    /// Example:
    /// @code
    ///      void on_my_signal(MappingSignal::MappingAction change,
    ///                        const std::string &key,
    ///                        std::shared_ptr<MyDataType> &data) {...}
    ///      ...
    ///      SharedMappingSignal<MyDataType> my_mapping_signal;
    ///      signal.connect(on_my_mapping_signal);
    ///      ...
    ///      auto mydata = std::make_shared<MyDataType>({...});
    ///      my_mapping_signal.emit(MAP_ADDITION, "key", mydata);
    ///      my_mapping_signal.emit(MAP_UPDATE, "key", mydata);
    ///      my_mapping_signal.clear("key");
    /// @endcode

    template <class DataType, class KeyType = std::string>
    class SharedMappingSignal : public BaseSignal
    {
        using SharedDataPtr = std::shared_ptr<DataType>;
        using Super = BaseSignal;

    public:
        using Slot = std::function<void(MappingAction, KeyType, SharedDataPtr)>;

        SharedMappingSignal(const std::string &id, bool caching = false)
            : Super(id, caching)
        {
        }

        /// @brief Indicate whether the specified signal handle is connected.
        /// @param[in] handle
        ///     Signal handle.
        /// @return
        ///     Boolean indicating whether the specified signal handle is registered.
        bool connected(const Handle &handle) const
        {
            return this->slots_.count(handle);
        }

        /// @brief Register a signal handler for this signal.
        /// @param[in] slot
        ///     A function invoked whenever signal data is emitted
        /// @param[in] emit_cached
        ///     Invoke handler immediately with previously cached values, if any
        /// @return
        ///     Unique handle which can later be used to disconnect
        Handle connect(const Slot &slot, bool emit_cached = true)
        {
            Handle handle(this->unique_handle());
            this->connect(handle, slot, emit_cached);
            return handle;
        }

        /// @brief Register a signal handler for this signal.
        /// @param[in] handle
        ///     Unique handle which can later be used to disconnect
        /// @param[in] slot
        ///     A function invoked whenever signal data is emitted
        /// @param[in] emit_cached
        ///     Invoke handler immediately with previously cached values, if any
        void connect(const Handle &handle, const Slot &slot, bool emit_cached = true)
        {
            std::scoped_lock lck(this->signal_mtx);
            this->slots_[handle] = slot;
            if (emit_cached)
            {
                this->emit_cached_to(handle, slot);
            }
        }

        /// @brief
        ///     Unregister a callback handler for signals of the provided template type.
        /// @param[in] handle
        ///     Identity identity of the callback handler to be removed.

        void disconnect(const Handle &handle) override
        {
            std::scoped_lock lck(this->signal_mtx);
            this->slots_.erase(handle);
        }

        /// @brief
        ///     Emit a signal to registered receivers of the provided data type.
        /// @param[in] action
        ///     What mapping action is being emitted: ADDITION, REMOVAL, UPDATE.
        /// @param[in] key
        ///     Mapping key
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted
        std::size_t emit(
            MappingAction action,
            const KeyType &key,
            const DataType &value)
        {
            std::scoped_lock lck(this->signal_mtx);
            if (this->caching())
            {
                switch (action)
                {
                case MAP_UPDATE:
                case MAP_ADDITION:
                    this->update_cache(key, value);
                    break;

                case MAP_REMOVAL:
                    this->cached_.erase(key);
                    break;

                default:
                    break;
                }
            }
            return this->sendall(action, key, value);
        }

        /// @brief
        ///     Emit a signal to registered receivers of the provided data type.
        /// @param[in] key
        ///     Mapping key
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted
        std::size_t emit(
            const KeyType &key,
            const SharedDataPtr &value)
        {
            MappingAction action(
                !value                 ? MAP_REMOVAL
                : !this->caching()     ? MAP_UPDATE
                : this->is_cached(key) ? MAP_UPDATE
                                       : MAP_ADDITION);

            return this->emit(action, key, value);
        }

        /// @brief
        ///     Emit signal only if the current value differs from the previous one.
        /// @note
        ///     This only works if the `cache` option is used.
        /// @param[in] key
        ///     Mapping key
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     Whether or not the signal was emitted.
        bool emit_if_changed(
            const KeyType &key,
            const SharedDataPtr &value)
        {
            std::scoped_lock lck(this->signal_mtx);
            MappingAction action(
                !this->caching()                                   ? MAP_NONE
                : (this->cached_.count(key) == 0)                  ? MAP_ADDITION
                : !types::equivalent(value, this->cached_.at(key)) ? MAP_UPDATE
                                                                   : MAP_NONE);
            if (action != MAP_NONE)
            {
                this->update_cache(key, value);
                this->sendall(action, key, value);
                return true;
            }
            else
            {
                return false;
            }
        }

        /// @brief
        ///     Emit a REMOVED signal.
        /// @param[in] key
        ///     Mapping key.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted
        std::size_t clear(const KeyType &key, const DataType &value = {})
        {
            return this->emit(MAP_REMOVAL, key, value);
        }

        /// @brief
        ///     Emit a REMOVED signal if \p key is still in in the signal cache
        /// @param[in] key
        ///     Mapping key.
        std::size_t clear_if_cached(const KeyType &key)
        {
            std::scoped_lock lck(this->signal_mtx);
            if (auto nh = this->cached_.extract(key))
            {
                return this->sendall(MAP_REMOVAL, key, nh.mapped());
            }
            else
            {
                return 0;
            }
        }

        /// @brief
        ///     Clean the cache, emitting a REMOVED signal for every item in it.
        std::size_t clear_all_cached()
        {
            std::scoped_lock lck(this->signal_mtx);
            std::size_t count = 0;
            for (const auto &[key, value] : this->cached_)
            {
                count += this->sendall(MAP_REMOVAL, key, value);
            }
            this->cached_.clear();
            return count;
        }

        /// @brief
        ///    Get the current cached value, if any.
        /// @return
        ///    std::optional<DataType> object
        types::ValueMap<KeyType, DataType> get_cached()
        {
            std::scoped_lock lck(this->signal_mtx);
            return this->cached_;
        }

        /// @brief
        ///    Get the most recent data value emitted for a given key
        /// @param[in] key
        ///    Mapping key
        /// @return
        ///    Most recent data value emitted for the specified key, if any
        SharedDataPtr get_cached(const std::string &key)
        {
            std::scoped_lock lck(this->signal_mtx);
            try
            {
                return this->cached_.at(key);
            }
            catch (const std::out_of_range &e)
            {
                return {};
            }
        }

        /// @brief
        ///    Get the most recent data value emitted for a given key
        /// @param[in] key
        ///    Mapping key
        /// @param[in] fallback
        ///    Data value to return if the specified key doesn't exist in the cache
        /// @return
        ///    Most recent data value emitted for the specified key, otherwise
        ///    the provided fallback value.
        DataType get_cached(const std::string &key, const DataType &fallback)
        {
            return this->get_cached(key).value_or(fallback);
        }

        /// @brief
        ///    Indicate whether the specified mapping currently exists in the cache
        /// @param[in] key
        ///    Mapping key
        /// @return
        ///    Boolean indicator of whether the specified key exists
        bool is_cached(const std::string &key) noexcept
        {
            std::scoped_lock lck(this->signal_mtx);
            return this->cached_.count(key);
        }

        /// @brief
        ///    Obtain number cache size
        /// @return
        ///    Number of key/value pairs in cahe
        std::size_t cache_size()
        {
            std::scoped_lock lck(this->signal_mtx);
            return this->cached_.size();
        }

        /// @brief
        ///    Obtain number of current connections.
        /// @return
        ///    Number of connected slots
        std::size_t connection_count() const override
        {
            return this->slots_.size();
        }

        /// @brief
        ///     Update cache, emit deltas as addition/update/removal signals
        /// @param[in] update
        ///     Updated map
        /// @return
        ///     The number of signals emitted
        template <class MapType>
        std::size_t synchronize(const MapType &update)
        {
            std::unordered_map<KeyType, SharedDataPtr> previous = this->get_cached();
            std::size_t count = 0;
            for (const auto &[key, value] : update)
            {
                if (auto nh = previous.extract(key))
                {
                    if (!types::equivalent(nh.mapped(), value))
                    {
                        this->emit(
                            value ? MAP_UPDATE : MAP_REMOVAL,
                            key,
                            value);
                        count++;
                    }
                }
                else if (value)
                {
                    this->emit(MAP_ADDITION, key, value);
                    count++;
                }
            }
            for (const auto &[key, value] : previous)
            {
                this->emit(MAP_REMOVAL, key, value);
                count++;
            }
            return count;
        }

    protected:
        void update_cache(const KeyType &key, const DataType &value)
        {
            this->cached_.insert_or_assign(key, value);
        }

        void emit_cached_to(const std::string &handle,
                            const Slot &callback)
        {
            for (const auto &pair : this->cached_)
            {
                this->callback(handle, callback, MAP_ADDITION, pair.first, pair.second);
            }
        }

        std::size_t sendall(MappingAction action,
                            const KeyType &key,
                            const DataType &value = {})
        {
            std::size_t count = 0;
            for (const auto &[receiver, method] : this->slots_)
            {
                count += this->callback(receiver, method, action, key, value);
            }
            return count;
        }

        bool callback(const std::string &receiver,
                      const Slot &method,
                      MappingAction action,
                      const KeyType &key,
                      const DataType &value)
        {
            return this->safe_invoke(
                str::format("%s(%r, %r, {...})", receiver, action, key),
                std::bind(method, action, key, value));
        }

    private:
        types::ValueMap<KeyType, DataType> cached_;
        std::unordered_map<std::string, Slot> slots_;
    };
}  // namespace cc::core::signal
