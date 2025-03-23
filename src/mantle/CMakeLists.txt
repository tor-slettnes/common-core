## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(LICENSE_FILE ${CC_PROJECT_DIR}/LICENSE.txt)

## Descend into subdirectories where the code is. We include components we have
## explicitly chosen not to build, because we may be needing their client
## libraries for other apps (either within this repo or in a parent repo for
## which this is a submodule). In that case we build only the libraries that are
## actually needed for those applications.

if(BUILD_DEMO)
  add_subdirectory(demo)
else()
  add_subdirectory(demo EXCLUDE_FROM_ALL)
endif()

if(BUILD_MULTILOGGER)
  add_subdirectory(multilogger)
else()
  add_subdirectory(multilogger EXCLUDE_FROM_ALL)
endif()

if(BUILD_SWITCHBOARD)
  add_subdirectory(switchboard)
else()
  add_subdirectory(switchboard EXCLUDE_FROM_ALL)
endif()

if(BUILD_PLATFORM)
  add_subdirectory(platform)
else()
  add_subdirectory(platform EXCLUDE_FROM_ALL)
endif()
