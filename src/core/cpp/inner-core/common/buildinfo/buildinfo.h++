// -*- c++ -*-
//==============================================================================
/// @file buildinfo.h++
/// @brief Build information
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include <string>
#include <cstdint>

extern const std::string PRODUCT_NAME;
extern const std::string PROJECT_NAME;
extern const std::string PROJECT_DESCRIPTION;
extern const std::string PROJECT_VERSION;

extern const std::uint32_t PROJECT_VERSION_MAJOR;
extern const std::uint32_t PROJECT_VERSION_MINOR;
extern const std::uint32_t PROJECT_VERSION_PATCH;
extern const std::uint32_t PROJECT_VERSION_TWEAK;
extern const std::uint32_t BUILD_NUMBER;
extern const std::uint32_t BUILD_TIME;

extern const std::string BUILD_HOST;
extern const std::string BUILD_OS;
extern const std::string BUILD_OS_RELEASE;
extern const std::string BUILD_OS_VERSION;
extern const std::string BUILD_DISTRIB_ID;
extern const std::string BUILD_DISTRIB_NAME;
extern const std::string BUILD_DISTRIB_VERSION;
extern const std::string BUILD_DISTRIB_VERSION_ID;
extern const std::string BUILD_DISTRIB_VERSION_CODENAME;
extern const std::string BUILD_DISTRIB_PRETTY_NAME;
extern const std::string SOURCE_DIR;
extern const std::string SETTINGS_DIR;
extern const std::string ORGANIZATION;
