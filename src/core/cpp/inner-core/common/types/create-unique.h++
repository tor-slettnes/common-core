/// -*- c++ -*-
//==============================================================================
/// @file create-unique.h++
/// @brief Add static `create_unique()` method to create unique_ptr<> instance
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <memory>

namespace core::types
{
    /// @class create_unique
    /// @brief Create a `unique_ptr<Class>` to a `Class` with protected constructor
    /// @tparam UniqueClass
    ///     UniqueClass

    template <class UniqueClass, class... Args>
    [[nodiscard]] std::unique_ptr<UniqueClass> create_unique(Args &&...args);

    /// @class enable_create_unique
    /// @tparam UniqueClass
    ///     Class which will get a static `::create_unique()` method
    /// @tparam Base
    ///     Type of std::unique_ptr<> instance returned by `::create_unique()`
    ///
    /// This adds a static public `::create_unique()` method to an arbitrary class T
    /// to return a new 'unique_ptr<T>' instance.
    ///
    /// `enable_create_unique<T>` is further derived from
    /// `std::enable_unique_from_this<T>`. As such, T's constructor should be kept
    /// protected to ensure every instance is a valid valid unique_ptr<T>. The
    /// constructor cannot be private because we derive a helper class from T in
    /// order to make it accessible to `std::make_unique<>`).
    ///
    /// Example:
    /// ```
    ///     class MyClass : public enable_create_unique<T>
    ///     {
    ///     protected:
    ///         MyClass(const std::string &foo, const std::string &bar);
    ///     };
    ///
    ///     std::unique_ptr<MyClass> my_unique = MyClass::create_unique("my foo", "my bar");
    /// ```

    template <class UniqueClass, class UniqueBase = UniqueClass>
    class enable_create_unique
    {
    public:
        template <class... Args>
        [[nodiscard]] static std::unique_ptr<UniqueBase> create_unique(Args &&...args);
    };

    template <class T>
    bool equivalent(const std::unique_ptr<T> &lhs, const std::unique_ptr<T> &rhs);
}  // namespace core::types

#include "create-unique.i++"
