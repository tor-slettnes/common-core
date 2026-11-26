/// -*- c++ -*-
//==============================================================================
/// @file void-signal.c++
/// @brief Adaptation of Signal/Slot pattern - value-less signal type
/// @author Tor Slettnes
//==============================================================================

#include "signal/void-signal.h++"

namespace cc::core::signal
{
    //==========================================================================
    /// @class VoidSignal
    /// @brief Signal without data

    VoidSignal::VoidSignal(const std::string &id)
        : Super(id, false),
          BinaryEvent(false)
    {
    }

    Handle VoidSignal::connect(const Slot &slot)
    {
        Handle handle(this->unique_handle());
        this->connect(handle, slot);
        return handle;
    }

    void VoidSignal::connect(const Handle &handle, const Slot &slot)
    {
        std::scoped_lock lck(this->signal_mtx);
        this->slots_[handle] = slot;
    }

    void VoidSignal::disconnect(const Handle &handle)
    {
        std::scoped_lock lck(this->signal_mtx);
        this->slots_.erase(handle);

        this->cancel();
    }

    std::size_t VoidSignal::emit()
    {
        this->set();
        std::size_t count = 0;

        std::scoped_lock lck(this->signal_mtx);
        for (const auto &[receiver, method] : this->slots_)
        {
            count += this->callback(receiver, method);
        }

        return count;
    }

    bool VoidSignal::emitted() const
    {
        return this->is_set();
    }

    std::size_t VoidSignal::connection_count() const
    {
        return this->slots_.size();
    }

    bool VoidSignal::callback(const std::string &receiver, const Slot &method)
    {
        return this->safe_invoke(receiver, method);
    }

}  // namespace cc::core::signal
