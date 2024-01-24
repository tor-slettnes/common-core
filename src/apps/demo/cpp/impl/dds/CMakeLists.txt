## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Demo application interface - DDS implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_IDL)
  add_subdirectory(types)
endif()

if(BUILD_RTI_DDS)
  ### Pub/Sub interface
  add_subdirectory(dds-publisher)
  add_subdirectory(dds-subscriber)

  ### RPC interface
  add_subdirectory(rpc-client)
  add_subdirectory(rpc-server)
endif()

