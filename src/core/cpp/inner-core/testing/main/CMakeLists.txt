## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build unit tests
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of the test.
set(TARGET cc_core_test_main)

add_library(${TARGET}
  main.c++
)

target_link_libraries(${TARGET}
  cc_core_platform
  GTest::gtest
)
