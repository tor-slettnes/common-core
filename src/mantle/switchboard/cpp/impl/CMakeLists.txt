## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build switchboard service implementations
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(native)
add_subdirectory(dds)
add_subdirectory(grpc)
