## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(core)     # Shared components (modules, libraries)
add_subdirectory(mantle)   # Common applications/microservices
