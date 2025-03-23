## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(LICENSE_FILE ${CC_PROJECT_DIR}/LICENSE.txt)

if(BUILD_DEMO)
  add_subdirectory(demo)
endif()

if(BUILD_MULTILOGGER)
  add_subdirectory(multilogger)
endif()

if(BUILD_SWITCHBOARD)
  add_subdirectory(switchboard)
endif()

if(BUILD_PLATFORM)
  add_subdirectory(platform)
endif()
