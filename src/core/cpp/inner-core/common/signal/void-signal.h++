/// -*- c++ -*-
//==============================================================================
/// @file void-signal.h++
/// @brief Adaptation of Signal/Slot pattern - value-less signal type
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "signal/base-signal.h++"
#include "thread/binaryevent.h++"

#include <unordered_map>

namespace cc::core::signal
{
    //==========================================================================
    /// @class VoidSignal
    /// @brief Event notification without data

    class VoidSignal : public BaseSignal,
                       public core::types::BinaryEvent
    {
        using Super = BaseSignal;

    public:
        using Slot = std::function<void()>;

        VoidSignal(const std::string &id);

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
        void disconnect(const Handle &handle) override;

        /// @brief
        ///     Emit a signal to registered receivers
        /// @return
        ///     The number of connected slots to which the signal was emitted
        virtual std::size_t emit();

        /// @brief
        ///     Check whether signal has been emitted since its creation
        /// @return
        ///     `true` if signal has been emitted.
        bool emitted() const;

        /// @brief
        ///    Obtain number of current connections.
        /// @return
        ///    Number of connected slots
        std::size_t connection_count() const override;

    protected:
        bool callback(const std::string &receiver, const Slot &method);

    protected:
        std::unordered_map<std::string, Slot> slots_;
    };

}  // namespace cc::core::signal
