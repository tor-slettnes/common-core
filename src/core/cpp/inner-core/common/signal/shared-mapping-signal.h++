/// -*- c++ -*-
//==============================================================================
/// @file mapping-signal.h++
/// @brief Signal with mappable data as payload
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "signal/mapping-signal.h++"
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
    class SharedMappingSignal
        : public MappingSignal<std::shared_ptr<DataType>, KeyType>
    {
        using SharedDataPtr = std::shared_ptr<DataType>;
        using Super = MappingSignal<SharedDataPtr, KeyType>;

        using Super::get_cached;
        using Super::MappingSignal;

        std::size_t emit(
            const KeyType &key,
            const SharedDataPtr &value) override
        {
            MappingAction action(
                !value                 ? MAP_REMOVAL
                : !this->caching()     ? MAP_UPDATE
                : this->is_cached(key) ? MAP_UPDATE
                                       : MAP_ADDITION);

            return this->emit(action, key, value);
        }

        bool emit_if_changed(
            const KeyType &key,
            const SharedDataPtr &value) override
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

        template <class MapType>
        std::size_t synchronize(const MapType &update)
        {
            std::unordered_map<KeyType, DataType> previous = this->get_cached();
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
    };
}  // namespace cc::core::signal
