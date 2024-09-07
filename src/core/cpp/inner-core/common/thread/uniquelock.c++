/// -*- c++ -*-
//==============================================================================
/// @file uniquelock.h++
/// @brief Convenience wrapper around std::unique_lock<std::mutex>;
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "uniquelock.h++"

namespace core::types
{
    UniqueLock::UniqueLock()
        : lck(this->mtx, std::defer_lock)
    {
    }

    UniqueLock::operator bool() const noexcept
    {
        return this->locked();
    }

    void UniqueLock::lock() noexcept
    {
        this->lck.lock();
    }

    bool UniqueLock::try_lock() noexcept
    {
        try
        {
            return this->lck.try_lock();
        }
        catch (...)
        {
            return false;
        }
    }

    bool UniqueLock::locked() const noexcept
    {
        return this->lck.owns_lock();
    }

    void UniqueLock::unlock(bool check) noexcept
    {
        if (!check || this->locked())
        {
            return this->lck.unlock();
        }
    }

}  // namespace core::types
