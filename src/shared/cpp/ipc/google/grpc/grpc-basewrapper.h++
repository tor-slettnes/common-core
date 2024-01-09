/// -*- c++ -*-
//==============================================================================
/// @file grpc-basewrapper.h++
/// @brief Common functionality wrappers for gRPC client and server modules
/// @author Tor Slettnes <tor@slett.net>
///
/// Class templates for gRPC services (client and server), including:
///  * Settings store in YourServiceName.json
///  * Status/error code wrappers
//==============================================================================

#pragma once
#include "grpc-status.h++"
#include "status/exceptions.h++"
#include "config/settingsstore.h++"
#include "platform/path.h++"
#include "logging/logging.h++"

#include <grpc++/grpc++.h>

namespace cc::grpc
{
    define_log_scope("service");

    // Keys to look up settings in ServiceName.json
    constexpr auto SETTINGS_FILE_COMMON = "common-services";
    constexpr auto SETTINGS_FILE_PRODUCT = "product-services";
    constexpr auto PERSONALITY_SECTION = "personalities";
    constexpr auto DEFAULT_SECTION = "defaults";
    constexpr auto PORT_OPTION = "port";
    constexpr auto HOST_OPTION = "host";
    constexpr auto BIND_OPTION = "interface";
    constexpr auto MAX_MESSAGE_SIZE = "max message size";

    extern std::shared_ptr<SettingsStore> service_settings;

    //==========================================================================
    /// @class WrapperBase
    /// @brief
    ///     General purpose mix-in base class for ClientWrapperBase and
    ///     ServerWrapperBase.

    class WrapperBase
    {
    protected:
        /// Constructor. Loads service-specific settings from corresponding SERVICENAME.json file,
        /// and initializes a default path to save data files.
        WrapperBase(const std::string &fullServiceName);

    public:
        /// @fn servicename
        /// @brief
        ///     Return the name of this service
        /// @param[in] full
        ///     Obtain the full name (`"package.Name"`), otherwise just the stem (`"Name"`).
        /// @return
        ///     Service name
        std::string servicename(bool full = false) const;

    protected:
        /// Sanitize a target address of the form [HOST][:PORT] (where either or both
        /// may be absent) by ensuring it contains both.
        ///
        /// If either [HOST] or [:PORT] is missing, the corresponding option from the
        /// settings file ("ServiceName.json") is used. If PORT is still missing but
        /// "servicename" is defined in the settings file, the corresponding value
        /// used to find the service port via netdb (i.e., /etc/services).  Finally,
        /// if all else fails, the missing attribute is populated from defaultHost or
        /// defaultPort, respectively.
        ///
        /// @param[in] target
        ///     Address to sanitize, normally provided as a command-line option.
        /// @param[in] hostOption
        ///     Key to locate the host name in the settings file
        /// @param[in] portOption
        ///     Key to locate the port number in the settings file
        /// @param[in] defaultHost
        ///     Fallback if host name is not provided nor found in settings file
        /// @param[in] defaultPort
        ///     Fallback if port number is not provided nor found in settings file
        /// @return
        ///     Sanitized address of the form HOST:PORT (where HOST may still be empty)

        std::string realaddress(const std::string &address,
                                const std::string &hostOption,
                                const std::string &portOption,
                                std::string defaultHost = "localhost",
                                unsigned int defaultPort = 8080) const;

        /// Check that a string value (presumably from a ProtoBuf message) is non-empty.
        /// @param[in] fieldname The name of the input field, used in the error message.
        /// @param[in] value String value to check
        /// @exception exception::InvalidArgument The provided value is empty.
        void require(const std::string &fieldname, const std::string &value) const;

        /// Check that a scalar value (presumably from a ProtoBuf message) is non-zero.
        /// @param[in] fieldname The name of the input field, used in the error message.
        /// @param[in] value Scalar value to check.
        /// @exception exception::InvalidArgument The provided value is zero.
        template <class T>
        void require(const std::string &fieldname, T value) const
        {
            if (!value)
            {
                throwf(exception::InvalidArgument, "%r is required", fieldname);
            }
        }

        /// Obtain max. message size configuration
        int max_message_size() const;

    private:
        // Get a specific setting.
        types::Value setting(const std::string &key,
                             const std::string &personality = {},
                             const types::Value &defaultValue = {}) const;

        // Split an address of the form [PERSONALITY@][HOST][:PORT] into
        // separate values
        void splitaddress(const std::string &address,
                          std::string *personality,
                          std::string *host,
                          uint *port) const;

        // Join host and port into a string of the form "host:port"
        std::string joinaddress(const std::string &host, uint port) const;

    private:
        const std::string fullServiceName_;
    };

}  // namespace cc::grpc
