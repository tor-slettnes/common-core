## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Avro tests
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of the test.
set(TARGET avro-test)

add_executable(${TARGET}
  test-avro.c++
)

target_link_libraries(${TARGET}
  cc_core_io_avro_protobuf
  cc_core_test_main
)

include(GoogleTest)
gtest_discover_tests(${TARGET})
