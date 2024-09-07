/// -*- c++ -*-
//==============================================================================
/// @file uniquelock.h++
/// @brief Convenience wrapper around std::unique_lock<std::mutex>;
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <mutex>

namespace core::types
{
    class UniqueLock
    {
        using Super = std::unique_lock<std::mutex>;

    public:
        UniqueLock();

        operator bool() const noexcept;

        void lock() noexcept;
        bool try_lock() noexcept;
        bool locked() const noexcept;
        void unlock(bool check=false) noexcept;

    private:
        std::mutex mtx;
        std::unique_lock<std::mutex> lck;
    };

}  // namespace core::types
