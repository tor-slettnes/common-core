/// -*- c++ -*-
//==============================================================================
/// @file signaltemplate.h++
/// @brief Simple non-buffered adaptation of Signal/Slot pattern
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "string/format.h++"

#include <string>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <optional>
#include <ostream>
#include <unordered_map>
#include <map>
#include <mutex>

namespace cc::signal
{
    //==========================================================================
    /// @class BaseSignal
    /// @brief Abstract/untyped base for Signal<T> and MappedSignal<T>

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
        std::string name_;
        bool caching_;
        bool asynchronous_;
    };

    //==========================================================================
    /// @class Signal
    /// @brief Template for emitting arbitrary data types as signals,
    ///    and to register receivers that will be notified on change.
    ///
    /// Example:
    /// @code
    ///      void on_my_signal(const MyDataType &signal_data) {...}
    ///      ...
    ///      Signal<MyDataType> my_signal;
    ///      signal.connect(on_my_signal);
    ///      ...
    ///      MyDataType mydata = {...};
    ///      my_signal.emit(mydata);
    /// @endcode

    template <class DataType>
    class Signal : public BaseSignal
    {
        using Super = BaseSignal;

    public:
        using Slot = std::function<void(const DataType &)>;

        Signal(const std::string &id, bool caching = false)
            : Super(id, caching)
        {
        }

        /// @fn connect()
        /// @brief Register a signal handler for signals of the provided template type.
        /// @param[in] id
        ///     Unique identity of the callback handler, to be used for
        ///     subsequent cancellation.
        /// @param[in] slot
        ///     A callback function, invoked whenever the signal is emitted
        /// @return
        ///     A reference to the std::function that was passed in.

        inline const Slot &connect(const std::string &id, const Slot &slot)
        {
            std::scoped_lock lck(this->mtx_);
            this->slots_[id] = slot;
            this->emit_cached_to(id, slot);
            return slot;
        }

        /// @fn disconnect()
        /// @brief
        ///     Unregister a handler for signals of the provided template type.
        /// @param[in] id
        ///     Identity of the handler to be removed.

        inline void disconnect(const std::string &id)
        {
            std::scoped_lock lck(this->mtx_);
            this->slots_.erase(id);
        }

        /// @fn emit()
        /// @brief
        ///     Emit a signal to registered receivers of the provided data type.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted

        inline size_t emit(const DataType &value)
        {
            std::scoped_lock lck(this->mtx_);
            if (this->caching_)
            {
                this->update_cache(value);
            }
            return this->sendall(value);
        }

        /// @fn emit_if_changed()
        /// @brief
        ///     Emit signal only if the current value differs from the previous one.
        /// @note
        ///     This only works if the `cache` option is used.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted

        inline size_t emit_if_changed(const DataType &value)
        {
            std::scoped_lock lck(this->mtx_);
            if (!this->caching_ ||
                !this->cached_ ||
                (value != *this->cached_))
            {
                if (this->caching_)
                {
                    this->update_cache(value);
                }
                return this->sendall(value);
            }
            else
            {
                return 0;
            }
        }

        /// @fn get_cached()
        /// @brief
        ///    Get the current cached value, if any.
        /// @return
        ///    std::optional<DataType> object

        inline std::optional<DataType> get_cached()
        {
            std::scoped_lock lck(this->mtx_);
            return this->cached_;
        }

        /// @fn connection_count()
        /// @brief
        ///    Obtain number of current connections.
        /// @return
        ///    Number of connected slots

        inline size_t connection_count()
        {
            return this->slots_.size();
        }

    protected:
        inline virtual void emit_cached_to(const std::string &id,
                                           const Slot &slot)
        {
            if (this->cached_.has_value())
            {
                this->callback(id, slot, this->cached_.value());
            }
        }

        inline size_t sendall(const DataType &value)
        {
            for (const auto &cb : this->slots_)
            {
                this->callback(cb.first, cb.second, value);
            }
            return this->slots_.size();
        }

        inline void update_cache(const DataType &value)
        {
            this->cached_ = value;
        }

        inline void callback(const std::string &receiver,
                             const Slot &method,
                             const DataType &value)
        {
            this->safe_invoke(str::format("%s({...})", receiver),
                              std::bind(method, value));
        }

    private:
        std::optional<DataType> cached_;
        std::unordered_map<std::string, Slot> slots_;
        std::recursive_mutex mtx_;
    };

    enum MappingChange
    {
        MAP_NONE,
        MAP_ADDITION,
        MAP_REMOVAL,
        MAP_UPDATE,
    };

    inline std::ostream &operator<<(std::ostream &stream, MappingChange change)
    {
        static const std::unordered_map<MappingChange, std::string> names = {
            {MAP_NONE, "MAP_NONE"},
            {MAP_ADDITION, "MAP_ADDITION"},
            {MAP_REMOVAL, "MAP_REMOVAL"},
            {MAP_UPDATE, "MAP_UPDATE"},
        };

        try
        {
            stream << names.at(change);
        }
        catch (const std::out_of_range &)
        {
            stream << str::format("(Invalid MappingChange %d)", static_cast<uint>(change));
        }

        return stream;
    }

    //==========================================================================
    /// @class MappedSignal
    /// @brief Abstract base for signals that emit mappable data alongside
    ///   with their mapping key and a change type (added, removed, updated).
    ///
    ///
    /// Example:
    /// @code
    ///      void on_my_signal(MappedSignal::MappingChange change,
    ///                       const std::string &key,
    ///                       const MyDataType &data) {...}
    ///      ...
    ///      MappedSignal<MyDataType> my_mapped_signal;
    ///      signal.connect(on_my_mapped_signal);
    ///      ...
    ///      MyDataType mydata = {...};
    ///      my_mapped_signal.emit(MAP_ADDITION, "key", mydata);
    ///      my_mapped_signal.emit(MAP_UPDATE, "key", mydata);
    ///      my_mapped_signal.clear("key");
    /// @endcode

    template <class DataType, class KeyType = std::string>
    class MappedSignal : public BaseSignal
    {
        using Super = BaseSignal;

    public:
        using Slot = std::function<void(MappingChange, const KeyType &, const DataType &)>;

        MappedSignal(const std::string &id, bool caching = false)
            : Super(id, caching)
        {
        }

        /// @fn connect()
        /// @brief Register a pair of signal handlers for this signal.
        /// @param[in] id
        ///     Unique identity of the callback handler, to be used for
        ///     subsequent cancellation.
        /// @param[in] slot
        ///     A function invoked whenever signal data is emitted
        inline const Slot &connect(const std::string &id, const Slot &slot)
        {
            std::scoped_lock lck(this->mtx_);
            this->slots_[id] = slot;
            this->emit_cached_to(id, slot);
            return slot;
        }

        /// @fn disconnect()
        /// @brief
        ///     Unregister a callback handler for signals of the provided template type.
        /// @param[in] id
        ///     Identity identity of the callback handler to be removed.

        inline void disconnect(const std::string &id)
        {
            std::scoped_lock lck(this->mtx_);
            this->slots_.erase(id);
        }

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
        inline size_t emit(MappingChange change, const KeyType &key, const DataType &value)
        {
            std::scoped_lock lck(this->mtx_);
            if (this->caching_)
            {
                switch (change)
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
            return this->sendall(change, key, value);
        }

        /// @brief
        ///     Emit a signal to registered receivers of the provided data type.
        /// @param[in] key
        ///     Mapping key
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted

        inline size_t emit(const KeyType &key, const DataType &value)
        {
            MappingChange change(!this->caching_        ? MAP_NONE
                                 : this->is_cached(key) ? MAP_UPDATE
                                                        : MAP_ADDITION);
            return this->emit(change, key, value);
        }

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

        inline size_t emit_if_changed(const KeyType &key, const DataType &value)
        {
            std::scoped_lock lck(this->mtx_);
            MappingChange change =
                !this->caching_                    ? MAP_NONE
                : (this->cached_.count(key) == 0)  ? MAP_ADDITION
                : (this->cached_.at(key) != value) ? MAP_UPDATE
                                                   : MAP_NONE;

            if (change != MAP_NONE)
            {
                this->update_cache(key, value);
                return this->sendall(change, key, value);
            }
            else
            {
                return 0;
            }
        }

        /// @fn clear()
        /// @brief
        ///     Emit a REMOVED signal.
        /// @param[in] key
        ///     Mapping key.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted

        inline size_t clear(const KeyType &key, const DataType &value = {})
        {
            return this->emit(MAP_REMOVAL, key, value);
        }

        /// @fn clear_if_cached()
        /// @brief
        ///     Emit a REMOVED signal if \p key is still in in the signal cache
        /// @param[in] key
        ///     Mapping key.
        inline size_t clear_if_cached(const KeyType &key)
        {
            std::scoped_lock lck(this->mtx_);
            if (auto nh = this->cached_.extract(key))
            {
                return this->sendall(MAP_REMOVAL, key, nh.mapped());
            }
            else
            {
                return 0;
            }
        }

        /// @fn get_cached()
        /// @brief
        ///    Get the current cached value, if any.
        /// @return
        ///    std::optional<DataType> object
        inline std::unordered_map<KeyType, DataType> get_cached()
        {
            std::scoped_lock lck(this->mtx_);
            return this->cached_;
        }

        inline std::optional<DataType> get_cached(const std::string &key)
        {
            std::scoped_lock lck(this->mtx_);
            try
            {
                return this->cached_.at(key);
            }
            catch (const std::out_of_range &e)
            {
                return {};
            }
        }

        inline DataType get_cached(const std::string &key, const DataType &fallback)
        {
            return this->get_cached(key).value_or(fallback);
        }

        inline bool is_cached(const std::string &key) noexcept
        {
            std::scoped_lock lck(this->mtx_);
            return this->cached_.count(key);
        }

        /// @fn connection_count()
        /// @brief
        ///    Obtain number of current connections.
        /// @return
        ///    Number of connected slots
        inline size_t connection_count()
        {
            return this->slots_.size();
        }

        /// @fn synchronize()
        /// @brief
        ///     Update cache, emit deltas as addition/update/removal signals
        /// @param[in] update
        ///     Updated map
        /// @return
        ///     The number of signals emitted

        template <class MapType>
        inline size_t synchronize(const MapType &update)
        {
            std::unordered_map<KeyType, DataType> previous = this->get_cached();
            size_t count = 0;
            for (const auto &[key, value] : update)
            {
                if (auto nh = previous.extract(key))
                {
                    if (nh.mapped() != value)
                    {
                        this->emit(MAP_UPDATE, key, value);
                        count++;
                    }
                }
                else
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
        inline void update_cache(const KeyType &key, const DataType &value)
        {
            this->cached_.insert_or_assign(key, value);
        }

        inline void emit_cached_to(const std::string &id,
                                   const Slot &callback)
        {
            for (const auto &pair : this->cached_)
            {
                this->callback(id, callback, MAP_ADDITION, pair.first, pair.second);
            }
        }

        inline size_t sendall(MappingChange change,
                              const KeyType &key,
                              const DataType &value = {})
        {
            for (const auto &cb : this->slots_)
            {
                this->callback(cb.first, cb.second, change, key, value);
            }
            return this->slots_.size();
        }

        inline void callback(const std::string &receiver,
                             const Slot &method,
                             MappingChange change,
                             const KeyType &key,
                             const DataType &value)
        {
            this->safe_invoke(str::format("%r (%r, %r, {...})", receiver, change, key),
                              std::bind(method, change, key, value));
        }

    private:
        std::recursive_mutex mtx_;
        std::unordered_map<KeyType, DataType> cached_;
        std::unordered_map<std::string, Slot> slots_;
    };
}  // namespace cc::signal
