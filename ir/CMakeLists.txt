file(GLOB_RECURSE lib-sources lib/*.cpp)
file(GLOB_RECURSE bin-sources src/*.cpp)

add_library(irlib STATIC ${lib-sources})
target_include_directories(irlib PUBLIC include)

add_executable(ir ${bin-sources})
target_include_directories(ir PRIVATE include)

target_link_libraries(ir PRIVATE irlib)
