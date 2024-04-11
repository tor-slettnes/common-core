## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build unit tests
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of the test.
set(TARGET core-tests)

set(LIB_DEPS
  cc_core_test_main
)

set(SOURCES
  test-string-misc.c++
  test-string-expand.c++
)

BuildExecutable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)

include(GoogleTest)
gtest_discover_tests(${TARGET})

