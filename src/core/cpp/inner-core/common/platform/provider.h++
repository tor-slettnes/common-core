/// -*- c++ -*-
//==============================================================================
/// @file provider.h++
/// @brief Abstract provider/consumer interface for system configuration
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/create-shared.h++"
#include "string/misc.h++"

#include <unordered_map>
#include <memory>
#include <string>

namespace core::platform
{
    class Provider;

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
            : name_(name),
              priority_(priority)
        {
        }

        virtual ~Provider() {}

    public:
        /// Determine if derived provider type is applicable on this host.
        virtual bool is_pertinent() const { return true; }
        virtual void initialize() {}
        virtual void deinitialize() {}
        std::string name() const { return this->name_; }
        ProviderPriority priority() const { return this->priority_; }

    private:
        const std::string name_;
        const ProviderPriority priority_;
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
                (!this->provider ||
                 (candidate->priority() > this->provider->priority())) &&
                candidate->is_pertinent();

            if (accepted)
            {
                this->provider = candidate;
                std::static_pointer_cast<Provider>(candidate)->initialize();
                return candidate;
            }
            else
            {
                return {};
            }
        }

        template <class ProviderType>
        inline bool unregisterProvider()
        {
            if (std::dynamic_pointer_cast<ProviderType>(this->provider))
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
                std::stringstream ss;
                ss << "No suitable "
                   << std::quoted(this->typeName)
                   << " implementation is available for this system";

                throw std::runtime_error(ss.str());
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
}  // namespace core::platform
