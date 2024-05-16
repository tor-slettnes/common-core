/// -*- c++ -*-
//==============================================================================
/// @file scope.c++
/// @brief Log scope
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "scope.h++"

namespace cc::logging
{
    //==========================================================================
    // Scope

    Scope::Scope(const std::string &name, status::Level threshold)
        : name(name),
          threshold(threshold)
    {
    }

    Scope::Ref Scope::create(const std::string &name, status::Level threshold)
    {
        auto [it, inserted] = scopes.try_emplace(name);
        if (inserted)
        {
            it->second = std::make_shared<Scope>(name, threshold);
        }
        return it->second;
    }

    status::Level Scope::effective_threshold() const
    {
        return (this->threshold != status::Level::NONE) ? this->threshold : default_threshold;
    }

    bool Scope::is_applicable(status::Level level) const
    {
        return ((level >= this->effective_threshold()) &&
                (this->effective_threshold() != status::Level::NONE));
    }

    //==========================================================================
    // Threshold

    void set_default_threshold(status::Level threshold)
    {
        logging::default_threshold = threshold;
    }

    void set_universal_threshold(status::Level threshold)
    {
        for (auto &[name, scope] : logging::scopes)
        {
            scope->threshold = threshold;
        }
    }

    status::Level current_threshold()
    {
        return log_scope->threshold;
    }

}  // namespace cc::logging
