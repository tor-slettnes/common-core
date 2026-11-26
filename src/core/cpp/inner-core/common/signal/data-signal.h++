/// -*- c++ -*-
//==============================================================================
/// @file data-signal.h++
/// @brief Signal with single value payload
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "signal/base-signal.h++"
#include <optional>

namespace cc::core::signal
{
    //==========================================================================
    /// @class DataSignal
    /// @brief
    ///    Template for emitting arbitrary data types as signals,
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
        using Slot = std::function<void(DataType)>;

        DataSignal(const std::string &id, bool caching = false)
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

        /// @brief Register a signal handler for signals of the provided template type.
        /// @param[in] slot
        ///     A callback function, invoked whenever the signal is emitted
        /// @param[in] emit_cached
        ///     Invoke handler immediately with previously cached value, if any
        /// @return
        ///     A unique handle, which can later be used to disconnect
        Handle connect(const Slot &slot, bool emit_cached = true)
        {
            Handle handle(this->unique_handle());
            this->connect(handle, slot, emit_cached);
            return handle;
        }

        /// @brief Register a signal handler for signals of the provided template type.
        /// @param[in] handle
        ///     Unique identity of the callback handler, to be used for
        ///     subsequent cancellation.
        /// @param[in] slot
        ///     A callback function, invoked whenever the signal is emitted
        /// @param[in] emit_cached
        ///     Invoke handler immediately with previously cached value, if any
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
        ///     Unregister a handler for signals of the provided template type.
        /// @param[in] handle
        ///     Identity of the handler to be removed.
        void disconnect(const Handle &handle) override
        {
            std::scoped_lock lck(this->signal_mtx);
            this->slots_.erase(handle);
        }

        /// @brief
        ///     Emit a signal to registered receivers of the provided data type.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     The number of connected slots to which the signal was emitted
        std::size_t emit(const DataType &value)
        {
            std::scoped_lock lck(this->signal_mtx);
            if (this->caching())
            {
                this->update_cache(value);
            }
            return this->sendall(value);
        }

        /// @brief
        ///     Emit signal only if the current value differs from the previous one.
        /// @note
        ///     This only works if the `cache` option is enabled in the constructor.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     Whether or not the signal was emitted.
        bool emit_if_changed(const DataType &value)
        {
            std::scoped_lock lck(this->signal_mtx);
            if (!this->caching() || !this->cached_ || !(value == *this->cached_))
            {
                if (this->caching())
                {
                    this->update_cache(value);
                }
                this->sendall(value);
                return true;
            }
            else
            {
                return false;
            }
        }

        /// @brief
        ///    Get the current cached value, if any.
        /// @return
        ///    std::optional<DataType> object
        std::optional<DataType> get_cached()
        {
            std::scoped_lock lck(this->signal_mtx);
            return this->cached_;
        }

        /// @brief
        ///    Get the current cached value if any, otherwise a fallback value.
        /// @param[in] fallback
        ///    Value to return if there's no cached value.
        /// @return
        ///    std::optional<DataType> object
        DataType get_cached(const DataType &fallback)
        {
            return this->cached_.value_or(fallback);
        }

        /// @brief
        ///    Clear any cached signal
        /// @return
        ///    Boolean indicator of whether a signal were held in cache prior to clearing.
        bool clear_cached()
        {
            std::scoped_lock lck(this->signal_mtx);
            bool exists = this->cached_.has_value();
            this->cached_.reset();
            return exists;
        }

        /// @brief
        ///    Obtain number of current connections.
        /// @return
        ///    Number of connected slots
        std::size_t connection_count() const override
        {
            return this->slots_.size();
        }

    protected:
        virtual void emit_cached_to(const std::string &handle,
                                    const Slot &slot)
        {
            if (this->cached_.has_value())
            {
                this->callback(handle, slot, this->cached_.value());
            }
        }

        std::size_t sendall(const DataType &value)
        {
            std::size_t count = 0;
            for (const auto &[receiver, method] : this->slots_)
            {
                count += this->callback(receiver, method, value);
            }
            return count;
        }

        void update_cache(const DataType &value)
        {
            this->cached_ = value;
        }

        bool callback(const std::string &receiver,
                      const Slot &method,
                      const DataType &value)
        {
            return this->safe_invoke(
                receiver + "({...})",
                std::bind(method, value));
        }

    private:
        std::optional<DataType> cached_;
        std::unordered_map<std::string, Slot> slots_;
    };

}  // namespace cc::core::signal
