/// -*- c++ -*-
//==============================================================================
/// @file create-shared.h++
/// @brief Add static `create_shared()` method to create shared_ptr<> instance
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <memory>

namespace core::types
{
    /// @class create_shared
    /// @brief Create a `shared_ptr<Class>` to a `Class` with protected constructor
    /// @tparam SharedClass
    ///     SharedClass

    template <class SharedClass, class... Args>
    [[nodiscard]] std::shared_ptr<SharedClass> create_shared(Args &&...args);

    /// @class enable_create_shared
    /// @tparam SharedClass
    ///     Class which will get a static `::create_shared()` method
    /// @tparam Base
    ///     Type of std::shared_ptr<> instance returned by `::create_shared()`
    ///
    /// This adds a static public `::create_shared()` method to an arbitrary class T
    /// to return a new 'shared_ptr<T>' instance.
    ///
    /// `enable_create_shared<T>` is further derived from
    /// `std::enable_shared_from_this<T>`. As such, T's constructor should be kept
    /// protected to ensure every instance is a valid valid shared_ptr<T>. The
    /// constructor cannot be private because we derive a helper class from T in
    /// order to make it accessible to `std::make_shared<>`).
    ///
    /// Example:
    /// ```
    ///     class MyClass : public enable_create_shared<T>
    ///     {
    ///     protected:
    ///         MyClass(const std::string &foo, const std::string &bar);
    ///     };
    ///
    ///     std::shared_ptr<MyClass> my_shared = MyClass::create_shared("my foo", "my bar");
    /// ```

    template <class SharedClass, class SharedBase = SharedClass>
    class enable_create_shared
    {
    public:
        using ptr = std::shared_ptr<SharedBase>;

        template <class... Args>
        [[nodiscard]] static std::shared_ptr<SharedBase> create_shared(Args &&...args);
    };

    template <class SharedClass, class SharedBase = SharedClass>
    class enable_create_shared_from_this : public std::enable_shared_from_this<SharedClass>,
                                           public enable_create_shared<SharedClass, SharedBase>
    {
    };

    template <class T>
    bool equivalent(const std::shared_ptr<T> &lhs, const std::shared_ptr<T> &rhs);
}  // namespace core::types

#include "create-shared.i++"
