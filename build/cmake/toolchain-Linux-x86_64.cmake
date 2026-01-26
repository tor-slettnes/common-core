set(LINUX YES)

string(TOLOWER "$<CONFIG>" buildconfig)
if("${buildconfig}" STREQUAL "debug")
  add_compile_options(-Wall -Wextra -Wno-unused -pedantic)
endif()
