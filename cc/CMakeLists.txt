file(GLOB_RECURSE lib-sources lib/*.cpp)
file(GLOB_RECURSE bin-sources src/*.cpp)

add_library(cc ${lib-sources})
target_include_directories(cc PUBLIC include)
target_link_libraries(cc PUBLIC common)

add_executable(scc ${bin-sources})
target_include_directories(scc PRIVATE include)

target_link_libraries(scc PRIVATE cc)
