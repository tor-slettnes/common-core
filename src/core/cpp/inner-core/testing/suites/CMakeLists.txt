## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build unit tests
## @author Tor Slettnes
#===============================================================================

### Name of the test.
set(TARGET core-tests)

add_executable(${TARGET}
  test-bytevector.c++
  test-string-misc.c++
  test-string-expand.c++
  test-variant.c++
  test-uuid.c++
)

target_link_libraries(${TARGET}
  cc_core_test_main
)

include(GoogleTest)
gtest_discover_tests(${TARGET})
