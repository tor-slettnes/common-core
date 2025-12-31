## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes
#===============================================================================

set(LICENSE_FILE ${CC_PROJECT_DIR}/LICENSE.txt)

add_subdirectory(demo)
add_subdirectory(multilogger)
add_subdirectory(switchboard)
add_subdirectory(platform)
