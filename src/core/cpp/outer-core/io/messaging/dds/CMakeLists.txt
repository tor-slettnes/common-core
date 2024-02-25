## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build IDL encoder/decoder methods for share data types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### RTI Data Distribution Service wrappers
if(BUILD_RTI_DDS)
  add_subdirectory(rti)
endif()


