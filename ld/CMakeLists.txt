file(GLOB_RECURSE lib-sources lib/*.cpp)
file(GLOB_RECURSE bin-sources src/*.cpp)

add_library(ldlib STATIC ${lib-sources})
target_include_directories(ldlib PUBLIC include)

add_executable(ld ${bin-sources})
target_include_directories(ld PRIVATE include)

target_link_libraries(ld PRIVATE ldlib)
