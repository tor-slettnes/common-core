## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build switchboard service implementations
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(types)
add_subdirectory(dds-publisher)
add_subdirectory(dds-subscriber)
add_subdirectory(rpc-service)
add_subdirectory(rpc-client)
