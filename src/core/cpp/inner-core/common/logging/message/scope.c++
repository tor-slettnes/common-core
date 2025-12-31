/// -*- c++ -*-
//==============================================================================
/// @file scope.c++
/// @brief Log scope
/// @author Tor Slettnes
//==============================================================================

#include "scope.h++"

namespace core::logging
{
    //==========================================================================
    // Scope

    Scope::Scope(const std::string &name, status::Level threshold)
        : name(name),
          threshold(threshold)
    {
    }

    Scope::ptr Scope::create(const std::string &name, status::Level threshold)
    {
        auto [it, inserted] = scopes.try_emplace(name);
        if (inserted)
        {
            it->second = std::make_shared<Scope>(name, threshold);
        }
        return it->second;
    }

    void Scope::set_default_threshold(status::Level threshold)
    {
        This::default_threshold = threshold;
    }

    void Scope::set_universal_threshold(status::Level threshold)
    {
        This::universal_threshold = threshold;
    }

    void Scope::clear_universal_threshold()
    {
        This::universal_threshold.reset();
    }

    status::Level Scope::effective_threshold() const
    {
        return This::universal_threshold          ? This::universal_threshold.value()
               : (this->threshold != Level::NONE) ? this->threshold
                                                  : This::default_threshold;
    }

    bool Scope::is_applicable(status::Level level) const
    {
        status::Level threshold = this->effective_threshold();
        return (level >= threshold) && (threshold != Level::NONE);
    }

    //==========================================================================
    // Threshold

    status::Level Scope::default_threshold = status::Level::DEBUG;
    std::optional<status::Level> Scope::universal_threshold = {};

} // namespace core::logging
