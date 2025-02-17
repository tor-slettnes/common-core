## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_common)

### Library type.  We use static linking for convenience.
cc_add_library(${TARGET}
  LIB_TYPE STATIC
)

## Include appropriate system libraries based on platform
if(PLATFORM_QNX)
  target_link_libraries(${TARGET} PRIVATE c++fs)
elseif(LINUX)
  target_link_libraries(${TARGET} PRIVATE stdc++fs)
elseif(WIN32)
  target_link_libraries(${TARGET} PRIVATE dbghelp)
endif()

### C++17 required for variant data type, parameter packs, std::filesystem
target_compile_features(${TARGET} PUBLIC cxx_std_17)

# Pull in sources from subdirectories
add_subdirectory(argparse)
add_subdirectory(buildinfo)
add_subdirectory(chrono)
add_subdirectory(io)
add_subdirectory(string)
add_subdirectory(parsers)
add_subdirectory(logging)
add_subdirectory(platform)
add_subdirectory(settings)
add_subdirectory(status)
add_subdirectory(thread)
add_subdirectory(types)

