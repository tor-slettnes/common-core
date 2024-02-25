## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build platform-specific code
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

## FIX: The `LINUX` variable is not available until CMake 3.25.
## For earlier versions, set it here.
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set(LINUX ON)
endif()

## Include appropriate system libraries based on platform
if(PLATFORM_QNX)
  add_subdirectory(posix)
  add_subdirectory(qnx)
  set(LIB_DEPS cc_core_platform_qnx)

elseif(LINUX)
  add_subdirectory(posix)
  add_subdirectory(linux)
  set(LIB_DEPS cc_core_platform_linux)

elseif(WIN32)
  add_subdirectory(windows)
  set(LIB_DEPS cc_core_platform_windows)

else()
  message(SEND_ERROR "Unsupported platform ${CMAKE_SYSTEM_NAME}")
endif()

### Create common platform library
set(TARGET cc_core_platform)
set(LIB_TYPE STATIC)
include(BuildLibrary)

### Application `init()` routine. We put it here instead of `core` beause the
### functions `register_providers()` and `unregister_providers()` on which it
### depends are defined within this library.
add_subdirectory(application)
