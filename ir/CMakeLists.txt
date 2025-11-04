file(GLOB_RECURSE lib-sources lib/*.cpp)
file(GLOB_RECURSE bin-sources src/*.cpp)

add_library(ir ${lib-sources})
target_include_directories(ir PUBLIC include)
target_link_libraries(ir PUBLIC common)

add_executable(sir ${bin-sources})
target_include_directories(sir PRIVATE include)

target_link_libraries(sir PRIVATE ir)
