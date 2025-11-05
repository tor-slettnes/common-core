## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build unit tests
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of the test.
set(TARGET cc_core_test_main)

### External package dependencies
set(PKG_DEPS gtest)

### Library dependencies
set(LIB_DEPS cc_core_platform)

### Source files
set(SOURCES
  main.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PKG_DEPS ${PKG_DEPS}
  SOURCES ${SOURCES}
)

