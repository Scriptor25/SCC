file(GLOB_RECURSE LIB_SOURCES lib/*.cpp)

add_library(lib_common ${LIB_SOURCES})
add_library(scc::common ALIAS lib_common)
target_include_directories(lib_common PUBLIC include)
set_target_properties(lib_common PROPERTIES OUTPUT_NAME "common")
