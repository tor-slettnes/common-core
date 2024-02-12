## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Abstract service interface, agnostic to communications bus
add_subdirectory(common)
add_subdirectory(dds)
add_subdirectory(google)
add_subdirectory(zeromq)
add_subdirectory(python)
#add_subdirectory(http)
