## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build DEMO application tests
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of the test.
set(TARGET demo-test)

add_executable(${TARGET}
  test-demo-fixture.c++
  test-demo-native.c++
)

target_link_libraries(${TARGET}
  cc_demo_native
  cc_demo_protobuf_types
  cc_core_test_main
)

include(GoogleTest)
gtest_discover_tests(${TARGET})
