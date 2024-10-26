/// -*- c++ -*-
//==============================================================================
/// @file scope.h++
/// @brief Log scope
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "status/level.h++"
#include "types/valuemap.h++"

#include <string>
#include <map>
#include <memory>

//==============================================================================
/// Define a log scope for a specifc programming scope (namespace, class).
///
/// \b Example:
/// \code
///    nameapace mynamespace
///    {
///        // Default logging scope for mynamespace
///        define_log_scope("MyNamespaceScope");
///
///        class MyTroublesomeClass
///        {
///            // Set logging scope with a default threshold for this class.
///            define_log_scope("MyClassScope", logging::INFO);
///
///            void my_instance_method ()
///            {
///                log_debug("This message is generated in scope MyClassScope. "
///                          "It won't be logged by default, unless the scope's "
///                          "is lowered via settings or command line option.");
///            }
///        };
///
///        void my_namesapce_function ()
///        {
///            log_info("This message is logged in MyNamespaceScope. "
///                     "This scope does not have its own threshold, "
///                     "so the default threshold is used.");
///        }
///    }
///
///    void my_global_function ()
///    {
///        log_debug("This messasge is generated in the global scope.");
///    }
/// \endcode
///
/// The logging threshold for a given scope may be specified in the application settings
/// file with the option `log-threshold-SCOPE`, e.g.:
/// \code
///     {
///          "log-MyNamespaceScope" : "debug",
///          "log-MyClassScope" : "info",
///          "log-global" : "debug",
///          "log-shared" : "info",
///     }
/// \endcode
///
/// It may also be overridden at run-time with corresponding command line option, e.g.:
/// \code
///    myapplication --log-threshold-MyNamespaceScope=DEBUG --log-threshold-MyClassScope=INFO
/// \endcode

#define define_log_scope(...)                             \
    inline static ::core::logging::Scope::ptr log_scope = \
        ::core::logging::Scope::create(__VA_ARGS__)

#define use_log_scope(other) \
    inline static ::core::logging::Scope::ptr log_scope = other

#define use_shared_scope() \
    use_log_scope(::core::log_scope)

namespace core::logging
{

    //==========================================================================
    /// \class Scope
    /// \brief Log scope for messages.
    ///
    /// A logging scope serves as a scope for log messages. Each scope has its
    /// own logging threshold, and its name is optionally included in the log
    /// output.

    class Scope
    {
        /// \param[in] name
        ///     Log scope name, used in log output, as well as to construct
        ///     corresponding settings and command line options.
        /// \param[in] theshold
        ///     Default logging threshold for this scope. If not specified, and
        ///     not overridden in application settings or on the command line,
        ///     `default_threshold` is used.

    public:
        using ptr = std::shared_ptr<Scope>;

    public:
        Scope(const std::string &name, status::Level threshold);

        status::Level effective_threshold() const;
        bool is_applicable(status::Level level) const;

        static ptr create(const std::string &name, status::Level threshold = status::Level::NONE);

    public:
        std::string name;
        status::Level threshold;
    };

    /// Default threshold.  This may be overridden by specific scopes (including Global).
    inline status::Level default_threshold = status::Level::NOTICE;
    inline types::ValueMap<std::string, Scope::ptr> scopes;

    void set_default_threshold(status::Level threshold);
    void set_universal_threshold(status::Level threshold);

    /// Methods
    status::Level current_threshold();

}  // namespace core::logging

namespace core
{
    define_log_scope("shared", status::Level::NOTICE);
}

/// Global log scope.  Inherit default threshold.
define_log_scope("global", core::status::Level::NONE);
