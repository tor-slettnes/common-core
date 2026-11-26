/// -*- c++ -*-
//==============================================================================
/// @file base-signal.h++
/// @brief Adaptation of Signal/Slot pattern - abstract base
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "signal/types.h++"
#include "types/streamable.h++"

#include <functional>
#include <mutex>

namespace cc::core::signal
{
    //==========================================================================
    /// @class BaseSignal
    /// @brief Abstract base for VoidSignal, DataSignal<T> and MappingSignal<T>

    class BaseSignal : types::Streamable
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
        bool caching() const;
        std::string name() const;
        virtual void disconnect(const Handle &handle) = 0;
        virtual std::size_t connection_count() const = 0;

    protected:
        void to_stream(std::ostream &stream) const override;
        Handle unique_handle() const;
        bool safe_invoke(const std::string &receiver,
                         const std::function<void()> &f);
        std::size_t collect_futures(Futures &futures);

    private:
        std::string name_;
        bool caching_;

    protected:
        std::recursive_mutex signal_mtx;
    };

}  // namespace cc::core::signal
