set(LINUX YES)

string(TOLOWER "$<CONFIG>" buildconfig)
if("${buildconfig}" STREQUAL "debug")
  add_compile_options(-Wall -Wextra -Wno-unused -pedantic)
endif()

if(BUILD_CPP_STATIC_LIBS)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  set(BUILD_SHARED_LIBS OFF)
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

