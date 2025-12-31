## -*- cmake -*-
#===============================================================================
## @brief CMake toolchain spec for cross-compiling for Windows with MinGW
## @author Tor Slettnes
#===============================================================================

### Target system
set(CMAKE_SYSTEM_NAME Windows)

### Add definitions command for diffent platforms So we can use macro definition
set(PLATFORM_WIN32 ON)

### "#ifdef PLATFORM_QNX" in source code
add_compile_definitions(
  PLATFORM_WIN32=1
)

if(WIN32)
  add_compile_definitions(
    _CRT_SECURE_NO_WARNINGS
  )
endif()


