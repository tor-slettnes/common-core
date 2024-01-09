##
## -*- cmake -*-
#===============================================================================
## @brief CMake toolchain spec for cross-compiling for Windows with MinGW
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Target system
set(CMAKE_SYSTEM_NAME Windows)

### Add definitions command for diffent platforms So we can use macro definition
set(PLATFORM_WIN32 ON)

### "#ifdef PLATFORM_QNX" in source code
add_definitions(
  -DPLATFORM_WIN32=1
)

if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()


