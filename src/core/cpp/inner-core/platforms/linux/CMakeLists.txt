## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build platform-specific code - POSIX version
## @author Tor Slettnes
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
  linux-process.c++
  # linux-time.c++
)

if(BUILD_DNSSD_AVAHI)
  list(APPEND PKG_DEPS
    avahi-client
  )

  list(APPEND SOURCES
    linux-dns-sd-avahi.c++
  )

  add_compile_definitions(BUILD_DNSSD_AVAHI=1)
endif()



### Build library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PKG_DEPS ${PKG_DEPS}
  SOURCES ${SOURCES}
)

set(ERRNOFILE   ${CMAKE_CURRENT_BINARY_DIR}/errno-list.gen)
set(ERRNOSCRIPT ${CC_BUILDSCRIPTS_DIR}/generate_errnos.py)

cmake_path(RELATIVE_PATH ERRNOFILE
  BASE_DIRECTORY ${CMAKE_BINARY_DIR}
  OUTPUT_VARIABLE errno_relpath)

find_package(Python3
  COMPONENTS Interpreter
)

if(Python3_Interpreter_FOUND)
  add_custom_command(
    OUTPUT ${ERRNOFILE}
    COMMAND "${Python3_EXECUTABLE}" ${ERRNOSCRIPT} --output ${ERRNOFILE} --sysroot "${BUILD_ROOT}/"
    COMMENT "Generating ${errno_relpath}"
  )
endif()

add_custom_target(errnos DEPENDS ${ERRNOFILE})
add_dependencies(${TARGET} errnos)

### Include binary output dir
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
