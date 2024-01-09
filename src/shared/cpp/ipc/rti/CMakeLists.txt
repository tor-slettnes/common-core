##
## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build IDL encoder/decoder methods for share data types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### RTI Data Distribution Service wrappers
add_subdirectory(dds)

### RTI IDL data type encoding/decoding
add_subdirectory(idl)

### Create interface library to encapsulate the above
set(TARGET cc_shared_ipc_rti)
set(LIB_TYPE INTERFACE)
set(LIB_DEPS
  cc_shared_ipc_rti_dds
  cc_shared_ipc_rti_idl)

include(BuildLibrary)

