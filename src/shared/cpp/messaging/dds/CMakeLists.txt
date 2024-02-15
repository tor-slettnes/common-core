## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build IDL encoder/decoder methods for share data types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_IDL)
  ### Comon IDL data type encoding/decoding
  add_subdirectory(idl)
endif()

### RTI Data Distribution Service wrappers
if(BUILD_RTI_DDS)
  add_subdirectory(rti)
endif()


