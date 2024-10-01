#===============================================================================
## @file build_args.cmake
## @brief Get values from CMake build arguments or defaults file
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

function(get_build_arg OUT_VAR)
  if(NOT ${OUT_VAR})
    if ($ENV{${OUT_VAR}})
      set("${OUT_VAR}" "$ENV{${OUT_VAR}}" PARENT_SCOPE)
    else()
      if(NOT DEFAULTS_FILE)
        message(FATAL_ERROR "Variable DEFAULTS_FILE was not defined.")
      elseif(NOT _DEFAULTS)
        file(STRINGS "${DEFAULTS_FILE}" _DEFAULTS NEWLINE_CONSUME)
        set(_DEFAULTS "${_DEFAULTS}" PARENT_SCOPE)
      endif()

      string(JSON _value
        ERROR_VARIABLE _error
        GET "${_DEFAULTS}" ${ARGN})

      if(_error STREQUAL "NOTFOUND")
        # No error
        set("${OUT_VAR}" "${_value}" PARENT_SCOPE)

      elseif(DEFAULTS_FILE AND NOT _error MATCHES "not found$" AND NOT _WARNED_DEFAULTS)
        ### JSON Parsing error (triggered only once)
        message(FATAL_ERROR "Unable to read build settings from ${DEFAULTS_FILE}: ${_error}")
        set(_WARNED_DEFAULTS TRUE PARENT_SCOPE)

      endif()
    endif()
  endif()
endfunction()

function(get_build_number OUT_VAR)
  if(NOT ${OUT_VAR})
    if ($ENV{${OUT_VAR}})
      set("${OUT_VAR}" "$ENV{${OUT_VAR}}" PARENT_SCOPE)
    else()
      math(EXPR _result "${LAST_BUILD}+1")
      set("${OUT_VAR}" "${_result}" PARENT_SCOPE)
    endif()
  endif()

  set(LAST_BUILD "${${OUT_VAR}}"
      CACHE INTERNAL "..." FORCE)
endfunction()

### Global build configuration
get_build_arg(PROJECT "project")
get_build_arg(VERSION "version")
get_build_arg(DESCRIPTION "description")
get_build_arg(HOME_PAGE "home page")
get_build_number(BUILD_NUMBER)

### Debian package settings
get_build_arg(PACKAGE_NAME_PREFIX        "package" "name prefix")
get_build_arg(PACKAGE_VENDOR             "package" "vendor")
get_build_arg(PACKAGE_CONTACT            "package" "contact")
get_build_arg(PACKAGE_SPLIT_BY_COMPONENT "package" "split by component")
get_build_arg(PACKAGE_SPLIT_BY_GROUP     "package" "split by group")


### External (Python package) generator settings
get_build_arg(PYTHON_NAMESPACE        "python" "namespace")
get_build_arg(PYTHON_DEPENDENCIES     "python" "dependencies")

### Software upgrade settings
get_build_arg(UPGRADE_PACKAGE_SUFFIX "upgrade" "package suffix")
get_build_arg(UPGRADE_SCAN_URL       "upgrade" "scan url")
get_build_arg(UPGRADE_VFS_CONTEXT    "upgrade" "vfs context")
get_build_arg(UPGRADE_KEYRING        "upgrade" "keyring")



### Timestamp
string(TIMESTAMP BUILD_DATE_UTC "%Y-%m-%d" UTC)
string(TIMESTAMP BUILD_TIME_UTC "%H:%M:%S" UTC)
string(TIMESTAMP BUILD_DATE_LOCAL "%Y-%m-%d")
string(TIMESTAMP BUILD_TIME_LOCAL "%H:%M:%S")
string(TIMESTAMP BUILD_TIMESTAMP "%s")

### Host information
cmake_host_system_information(RESULT BUILD_HOST QUERY HOSTNAME)
cmake_host_system_information(RESULT BUILD_OS QUERY OS_NAME)
cmake_host_system_information(RESULT BUILD_OS_RELEASE QUERY OS_RELEASE)
cmake_host_system_information(RESULT BUILD_OS_VERSION QUERY OS_VERSION)
cmake_host_system_information(RESULT BUILD_DISTRIB_ID QUERY DISTRIB_ID)
cmake_host_system_information(RESULT BUILD_DISTRIB_NAME QUERY DISTRIB_NAME)
cmake_host_system_information(RESULT BUILD_DISTRIB_VERSION QUERY DISTRIB_VERSION)
cmake_host_system_information(RESULT BUILD_DISTRIB_VERSION_ID QUERY DISTRIB_VERSION_ID)
cmake_host_system_information(RESULT BUILD_DISTRIB_VERSION_CODENAME QUERY DISTRIB_VERSION_CODENAME)
cmake_host_system_information(RESULT BUILD_DISTRIB_PRETTY_NAME QUERY DISTRIB_PRETTY_NAME)
