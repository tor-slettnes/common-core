## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_YAML)
  target_sources(${TARGET} PRIVATE
    reader.c++
  )

  add_subdirectory(libyaml)
endif()
