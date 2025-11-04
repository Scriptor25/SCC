file(GLOB_RECURSE lib-sources lib/*.cpp)
file(GLOB_RECURSE bin-sources src/*.cpp)

add_library(as ${lib-sources})
target_include_directories(as PUBLIC include)
target_link_libraries(as PUBLIC common)

add_executable(sas ${bin-sources})
target_include_directories(sas PRIVATE include)

target_link_libraries(sas PRIVATE as)
