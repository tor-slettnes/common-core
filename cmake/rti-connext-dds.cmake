##
## -*- cmake -*-
#===============================================================================
## @file rti-connext-dds.cmake
## @brief Build with RTI Connext DDS
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### CMake 3.15 or newer required for Connext DDS logging macros
cmake_minimum_required(VERSION 3.15)

### RTI DDS version
set(CONNEXTDDS_VERSION 6.1.2)

### RTI DDS root folder (base name)
set(CONNEXTDDS_DIR_BASE rti_connext_dds-${CONNEXTDDS_VERSION})

### Determine search path in which to look for the RTI Connext DDS install folder
if($ENV{NDDSHOME})
  set(CONNEXTDDS_SEARCH_PATH
    "$ENV{NDDSHOME}"
  )

elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
  set(CONNEXTDDS_SEARCH_PATH
    "C:/Program Files (x86)"
    "C:/Program Files"
    "C:"
  )

elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin") # MacOS
  set(CONNEXTDDS_SEARCH_PATH
    "$ENV{HOME}/Applications"
    "$ENV{HOME}"
    "/Applications"
  )

else() # UNIX, Linux
  set(CONNEXTDDS_SEARCH_PATH
    ## UNIX/Linux
    "$ENV{HOME}"
    "/opt"
  )
endif()

### Search for RTI installation in the search path established above
if(NOT DEFINED CONNEXTDDS_DIR)
  foreach (candidate_folder ${CONNEXTDDS_SEARCH_PATH})
    if(IS_DIRECTORY ${candidate_folder}/${CONNEXTDDS_DIR_BASE})
      set(CONNEXTDDS_DIR ${candidate_folder}/${CONNEXTDDS_DIR_BASE})
    endif()
  endforeach()
endif()

if(NOT DEFINED CONNEXTDDS_DIR)
  message(FATAL_ERRROR
    " Could not locate RTI DDS ${CONNEXTDDS_VERSION} directory.  Try using -DCONNEXTDDS_DIR=/path/to/folder.")
endif()

### Guess RTI DDS target architecture in case we're cross-compiling
file(GLOB architectures_installed
  RELATIVE "${CONNEXTDDS_DIR}/lib"
  "${CONNEXTDDS_DIR}/lib/*")

string(SUBSTRING "${CMAKE_SYSTEM_NAME}" 0 3 short_system_name)
foreach(architecture_name ${architectures_installed})
  if("${architecture_name}" MATCHES "${short_system_name}")
    set(CONNEXTDDS_ARCH ${architecture_name})
  endif()
endforeach()

### Add RTI-provided CMake folder to module path
list(APPEND CMAKE_MODULE_PATH
  ${CONNEXTDDS_DIR}/resource/cmake)

### Find Connext DDS packages
find_package(RTIConnextDDS
  EXACT ${CONNEXTDDS_VERSION}
  REQUIRED)
