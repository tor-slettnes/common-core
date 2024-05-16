/// -*- c++ -*-
//==============================================================================
/// @file provider.h++
/// @brief Abstract provider/consumer interface for system configuration
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/create-shared.h++"
#include "string/misc.h++"
#include "string/format.h++"
#include "status/exception.h++"

#include <unordered_map>
#include <memory>
#include <string>

namespace cc::platform
{
    class Provider;
    using ProviderRef = std::shared_ptr<Provider>;

    //==========================================================================
    // Provider priority
    enum ProviderPriority
    {
        PRIORITY_LOW,
        PRIORITY_NORMAL,
        PRIORITY_HIGH
    };

    //==========================================================================
    /// @class Provider
    /// @brief Abstract base for platform interfaces (e.g., time, netifs, etc),
    ///    which in turn are abstract bases for specific providers (POSIX, GRPC).
    /// @param[in] name
    ///    Provider name (e.g. "PosixTimeProvider"), for logging/diagnostics.
    /// @param[in] priority
    ///    If a provider for a particular domain is already registered, it
    ///    will be replaced by this candidate if and only if the latter has
    ///    higher priority.

    class Provider
    {
    protected:
        Provider(const std::string &name,
                 ProviderPriority priority = PRIORITY_NORMAL)
            : name(name),
              priority(priority)
        {
        }

        virtual ~Provider() = default;

    public:
        /// Determine if derived provider type is applicable on this host.
        virtual bool is_pertinent() { return true; }
        virtual void initialize() {}
        virtual void deinitialize() {}

    public:
        const std::string name;
        const ProviderPriority priority;
    };

    //==========================================================================
    /// @class ProviderProxy
    /// @brief A forwarding proxy for the selected provider of a given domain.

    template <class Domain>
    class ProviderProxy
    {
    public:
        ProviderProxy(const std::string &typeName)
            : typeName(typeName)
        {
        }

        virtual ~ProviderProxy()
        {
        }

        template <class ProviderType, class... Args>
        inline std::shared_ptr<ProviderType> registerProvider(Args &&...args)
        {
            auto candidate =
                types::create_shared<ProviderType>(std::forward<Args>(args)...);

            bool accepted =
                (!this->provider || (candidate->priority > this->provider->priority)) &&
                candidate->is_pertinent();

            if (accepted)
            {
                this->provider = candidate;
                candidate->initialize();
                return candidate;
            }
            else
            {
                return {};
            }
        }

        template <class Provider>
        inline bool unregisterProvider()
        {
            if (std::dynamic_pointer_cast<Provider>(this->provider))
            {
                this->provider->deinitialize();
                this->provider.reset();
                return true;
            }
            else
            {
                return false;
            }
        }

        inline std::shared_ptr<Domain> get_shared() const
        {
            return this->provider;
        }

        inline Domain *get() const
        {
            if (auto ref = this->provider)
            {
                return ref.get();
            }
            else
            {
                throwf(std::runtime_error,
                       "No suitable %s provider is available for this system",
                       this->typeName);
            }
        }

        inline Domain &operator*() const
        {
            return *this->get();
        }

        inline Domain *operator->() const
        {
            return this->get();
        }

        inline operator bool() const noexcept
        {
            return bool(this->provider);
        }

    private:
        std::string typeName;
        std::shared_ptr<Domain> provider;
    };
}  // namespace cc::platform
