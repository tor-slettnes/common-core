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

set(TARGET cc_core_platform)

### Create common platform library
build_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS cc_core_common
)

## Include appropriate system libraries based on platform
if(PLATFORM_QNX)
  add_subdirectory(posix)
  add_subdirectory(qnx)
  target_link_libraries("${TARGET}" PRIVATE cc_core_platform_qnx)

elseif(LINUX)
  add_subdirectory(posix)
  add_subdirectory(linux)
  target_link_libraries("${TARGET}" PRIVATE cc_core_platform_linux)

elseif(WIN32)
  add_subdirectory(windows)
  target_link_libraries("${TARGET}" PRIVATE cc_core_platform_windows)

else()
  message(SEND_ERROR "Unsupported platform ${CMAKE_SYSTEM_NAME}")
endif()

### Application `init()` routine. We put it here instead of `core` beause the
### functions `register_providers()` and `unregister_providers()` on which it
### depends are defined within this library.
add_subdirectory(application)
