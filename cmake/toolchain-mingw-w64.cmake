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
  -DWIN32=1
  -DMINGW32=1)

### Compiler configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc-posix)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++-posix)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}-ar)

set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

set(CMAKE_FIND_ROOT_PATH
  /usr/${TOOLCHAIN_PREFIX}
)

### Include directories
#include_directories("/usr/x86_64-w64-mingw32/include")

