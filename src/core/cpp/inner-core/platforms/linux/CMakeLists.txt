## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build platform-specific code - POSIX version
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_platform_linux)

### Library dependencies
set(LIB_DEPS
  cc_core_platform_posix
)

### External package dependencies
set(PKG_DEPS
  uuid
)

### Sources
set(SOURCES
  linux-providers.c++
  linux-symbols.c++
  linux-host.c++
  linux-path.c++
  linux-time.c++
)

### Build library
include(BuildLibrary)
BuildLibrary("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  PKG_DEPS ${PKG_DEPS}
  SOURCES ${SOURCES}
)

set(ERRNOFILE   ${CMAKE_CURRENT_BINARY_DIR}/errno-list.gen)
set(ERRNOSCRIPT ${BUILD_SCRIPTS_DIR}/generate_errnos.py)
add_custom_command(
  OUTPUT ${ERRNOFILE}
  COMMAND python3 ${ERRNOSCRIPT} --output ${ERRNOFILE} --sysroot "${BUILD_ROOT}/"
  COMMENT "Generating ${ERRNOFILE}"
)

add_custom_target(errnos DEPENDS ${ERRNOFILE})
add_dependencies(${TARGET} errnos)

### Include binary output dir
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
