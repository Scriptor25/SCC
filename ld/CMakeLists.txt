file(GLOB_RECURSE lib-sources lib/*.cpp)
file(GLOB_RECURSE bin-sources src/*.cpp)

add_library(ld ${lib-sources})
target_include_directories(ld PUBLIC include)
target_link_libraries(ld PUBLIC common)

add_executable(sld ${bin-sources})
target_include_directories(sld PRIVATE include)

target_link_libraries(sld PRIVATE ld)
