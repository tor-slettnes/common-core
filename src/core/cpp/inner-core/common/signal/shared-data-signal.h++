/// -*- c++ -*-
//==============================================================================
/// @file shared-data-signal.h++
/// @brief Signal with `std::shared_ptr<>` payload
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "signal/data-signal.h++"
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
    class SharedDataSignal : public DataSignal<std::shared_ptr<DataType>>
    {
        using Super = DataSignal<std::shared_ptr<DataType>>;

    public:
        using SharedDataPtr = std::shared_ptr<DataType>;
        using Slot = std::function<void(SharedDataPtr)>;

        using Super::DataSignal;
        using Super::emit;
        using Super::get_cached;

        std::size_t emit(const DataType &value)
        {
            return this->emit(std::make_shared<DataType>(value));
        }

        bool emit_if_changed(const SharedDataPtr &value) override
        {
            std::scoped_lock lck(this->signal_mtx);
            if (!this->caching() ||
                !this->cached_ ||
                !core::types::equivalent(value, this->cached_))
            {
                return this->emit(value);
            }
            else
            {
                return false;
            }
        }

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
    };

}  // namespace cc::core::signal
