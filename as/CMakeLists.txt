file(GLOB_RECURSE lib-sources lib/*.cpp)
file(GLOB_RECURSE bin-sources src/*.cpp)

add_library(aslib STATIC ${lib-sources})
target_include_directories(aslib PUBLIC include)

add_executable(as ${bin-sources})
target_include_directories(as PRIVATE include)

target_link_libraries(as PRIVATE aslib)
