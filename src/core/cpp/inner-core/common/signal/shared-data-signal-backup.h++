/// -*- c++ -*-
//==============================================================================
/// @file shared-data-signal.h++
/// @brief Signal with `std::shared_ptr<>` payload
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "signal/base-signal.h++"
#include "types/create-shared.h++"

namespace cc::core::signal
{
    //==========================================================================
    /// @class SharedDataSignal
    /// @brief
    ///    Template for emitting `std::shared_ptr<>` types  as signals,
    ///    and to register receivers that will be notified on change.
    ///
    /// Example:
    /// @code
    ///      void on_my_signal(const MyDataType &signal_data) {...}
    ///      ...
    ///      SharedDataSignal<MyDataType> my_signal;
    ///      signal.connect(on_my_signal);
    ///      ...
    ///      auto mydata = std::make_shared<MyDataType>(...);
    ///      my_signal.emit(mydata);
    /// @endcode

    template <class DataType>
    class SharedDataSignal : public BaseSignal
    {
        using Super = BaseSignal;

    public:
        using SharedDataPtr = std::shared_ptr<DataType>;
        using Slot = std::function<void(SharedDataPtr)>;

        SharedDataSignal(const std::string &id, bool caching = false)
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
        std::size_t emit(const SharedDataPtr &value)
        {
            std::scoped_lock lck(this->signal_mtx);
            if (this->caching())
            {
                this->update_cache(value);
            }
            return this->sendall(value);
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
            auto value_ptr = std::make_shared<DataType>(value);
            if (this->caching())
            {
                this->update_cache(value_ptr);
            }
            return this->sendall(value_ptr);
        }

        /// @brief
        ///     Emit signal only if the current value differs from the previous one.
        /// @note
        ///     This only works if the `cache` option is enabled in the constructor.
        /// @param[in] value
        ///     Signal value.
        /// @return
        ///     Whether or not the signal was emitted.
        bool emit_if_changed(const SharedDataPtr &value)
        {
            std::scoped_lock lck(this->signal_mtx);
            if (!this->caching() || !this->cached_ || !core::types::equivalent(value, this->cached_))
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
        ///    Get the current cached value if set, otherwise a (possibly empty) fallback value
        /// @param[in] fallback
        ///    Value to return if there's no cached value. Empty by default.
        /// @return
        ///    std::shared_ptr<DataType> object
        SharedDataPtr get_cached(const SharedDataPtr &fallback = {})
        {
            std::scoped_lock lck(this->signal_mtx);
            return this->cached_ ? this->cached_ : fallback;
        }

        /// @brief
        ///    Clear any cached signal
        /// @return
        ///    Boolean indicator of whether a signal were held in cache prior to clearing.
        bool clear_cached()
        {
            std::scoped_lock lck(this->signal_mtx);
            bool exists = bool(this->cached_);
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
            if (this->cached_)
            {
                this->callback(handle, slot, this->cached_);
            }
        }

        std::size_t sendall(const SharedDataPtr &value)
        {
            std::size_t count = 0;
            for (const auto &[receiver, method] : this->slots_)
            {
                count += this->callback(receiver, method, value);
            }
            return count;
        }

        void update_cache(const SharedDataPtr &value)
        {
            this->cached_ = value;
        }

        bool callback(const std::string &receiver,
                      const Slot &method,
                      const SharedDataPtr &value)
        {
            return this->safe_invoke(
                receiver + "({...})",
                std::bind(method, value));
        }

    private:
        SharedDataPtr cached_;
        std::unordered_map<std::string, Slot> slots_;
    };

}  // namespace cc::core::signal
