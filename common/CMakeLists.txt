file(GLOB_RECURSE LIB_SOURCES lib/*.cpp)

add_library(scc_common ${LIB_SOURCES})
add_library(scc::common ALIAS scc_common)
target_include_directories(scc_common PUBLIC include)
