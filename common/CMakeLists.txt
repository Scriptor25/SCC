file(GLOB_RECURSE lib-sources lib/*.cpp)

add_library(common ${lib-sources})
target_include_directories(common PUBLIC include)
